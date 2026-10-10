"""Serial conversion publication tests using private mock catalogs and the built SQL test runner."""

import argparse
import hashlib
import json
import subprocess
import tempfile
import textwrap
import unittest
from pathlib import Path

from .config import load_config
from .model import CatalogError
from .server import MockServer

ROOT = Path(__file__).resolve().parents[2]


class ConversionTestCase(unittest.TestCase):
    binary = ROOT / "build/reldebug/test/unittest"

    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix="ducklake-conversion-")
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name)
        self.server = MockServer(self.path)
        self.server.__enter__()
        self.addCleanup(self.server.__exit__, None, None, None)
        config, uri = load_config()
        config["on_init"] = config["on_init"].replace(uri, self.server.uri)
        self.config = self.path / "config.json"
        self.config.write_text(json.dumps(config))

    def run_sql(self, body):
        source = str(self.path / "source.ducklake").replace("'", "''")
        data = str(self.path / "source_files").replace("'", "''")
        preamble = f"""
        require-env CATALOG_TEST_CONFIG_SETUP

        include test/sql/init_requires

        require ducklake

        statement ok
        ATTACH 'ducklake:{source}' AS lake (DATA_PATH '{data}', DATA_INLINING_ROW_LIMIT 0);
        """
        script = textwrap.dedent(preamble).strip() + "\n\n" + textwrap.dedent(body).strip() + "\n\n"
        result = subprocess.run(
            [str(self.binary), "--stdin", "--test-config", str(self.config)],
            input=script,
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=120,
        )
        output = result.stdout + result.stderr
        self.assertEqual(result.returncode, 0, output + "\nSQL:\n" + script)
        self.assertNotIn("All tests were skipped", output)
        self.assertNotIn("No tests ran", output)
        self.assertFalse(self.server.errors, "\n".join(self.server.errors))


class ConversionPublicationTests(ConversionTestCase):
    def test_case_colliding_iceberg_tables_are_rejected_before_import(self):
        self.run_sql(
            """
            statement ok
            CREATE TABLE my_datalake.default.case_collision (id INTEGER);
            """
        )
        schema = self.server.catalog.load((("default",), "case_collision"))["metadata"]["schemas"][0]
        with self.server.catalog.lock:
            self.server.catalog.create(
                ("default",), {"name": "CASE_COLLISION", "schema": schema, "stage-create": False}
            )
        self.run_sql(
            """
            statement error
            CALL iceberg_to_ducklake('my_datalake', 'lake');
            ----
            case-colliding destination table

            query I
            SELECT count(*) FROM __ducklake_metadata_lake.ducklake_table;
            ----
            0
            """
        )

    def test_expired_parent_preserves_existing_manifest_entries(self):
        self.run_sql(
            """
            statement ok
            CREATE TABLE my_datalake.default.expired_parent (id INTEGER);

            statement ok
            INSERT INTO my_datalake.default.expired_parent VALUES (1);

            statement ok
            INSERT INTO my_datalake.default.expired_parent VALUES (2);

            statement ok
            INSERT INTO my_datalake.default.expired_parent VALUES (3);
            """
        )
        key = (("default",), "expired_parent")
        metadata = self.server.catalog.load(key)["metadata"]
        oldest = min(metadata["snapshots"], key=lambda snapshot: snapshot["sequence-number"])["snapshot-id"]
        metadata["snapshots"] = [snapshot for snapshot in metadata["snapshots"] if snapshot["snapshot-id"] != oldest]
        metadata["snapshot-log"] = [entry for entry in metadata["snapshot-log"] if entry["snapshot-id"] != oldest]
        # Seed expiration through the mock's normal validated metadata publication.
        with self.server.catalog.lock:
            self.server.catalog.publish(key, metadata)
        self.run_sql(
            """
            require json

            query I
            CALL iceberg_to_ducklake('my_datalake', 'lake', tables := ['default.expired_parent']);
            ----
            1

            query II
            SELECT count(*), sum(id) FROM lake.default.expired_parent;
            ----
            3	6

            query I
            SELECT count(*) FROM __ducklake_metadata_lake.ducklake_snapshot_changes
            WHERE try_cast(commit_extra_info::JSON ->> 'snapshot-id' AS BIGINT) > 0;
            ----
            2
            """
        )

    def test_nested_namespaces_and_subsequent_writes(self):
        self.run_sql(
            f"""
            statement ok
            CREATE SCHEMA lake.parent;

            statement ok
            CREATE SCHEMA lake.parent.child;

            statement ok
            CREATE TABLE lake.parent.child.nested_table AS SELECT 42::INTEGER id;

            query I
            CALL ducklake_to_iceberg('lake', 'my_datalake', tables := ['parent.child.nested_table']);
            ----
            1

            query I
            SELECT id FROM my_datalake."parent.child".nested_table;
            ----
            42

            statement ok
            ATTACH 'ducklake:{self.path}/nested_back.ducklake' AS nested_back (DATA_PATH '{self.path}/nested_back_files', DATA_INLINING_ROW_LIMIT 0);

            query I
            CALL iceberg_to_ducklake('my_datalake', 'nested_back', tables := ['parent.child.nested_table']);
            ----
            1

            query I
            SELECT id FROM nested_back.parent.child.nested_table;
            ----
            42

            statement ok
            INSERT INTO nested_back.parent.child.nested_table VALUES (43);

            statement ok
            DROP TABLE my_datalake."parent.child".nested_table;

            query I
            CALL ducklake_to_iceberg('nested_back', 'my_datalake', tables := ['parent.child.nested_table']);
            ----
            1

            query I rowsort
            SELECT id FROM my_datalake."parent.child".nested_table;
            ----
            42
            43
            """
        )

    def test_preflight_does_not_stage_compatible_tables_before_an_incompatible_table(self):
        self.run_sql(
            """
            statement ok
            CREATE TABLE lake.a_compatible AS SELECT 1::INTEGER id;

            statement ok
            CREATE TABLE lake.z_incompatible (id UHUGEINT);

            statement error
            CALL ducklake_to_iceberg('lake', 'my_datalake');
            ----
            lossless Iceberg mapping

            query I
            SELECT id FROM lake.a_compatible;
            ----
            1
            """
        )
        self.assertFalse(self.server.catalog.tables)
        events = [json.loads(line) for line in self.server.journal.read_text().splitlines()]
        self.assertFalse([event for event in events if event["method"] == "POST" and event["path"].endswith("/tables")])

    def test_partial_publication_cleanup_and_explicit_retry(self):
        original = self.server.catalog.commit
        rejected_files = []
        source_hashes = {}
        calls = {}

        def commit(key, body):
            calls[key[1]] = calls.get(key[1], 0) + 1
            if key[1] == "z_failed" and calls[key[1]] == 1:
                for update in body["updates"]:
                    if update["action"] == "add-snapshot":
                        folder = Path(update["snapshot"]["manifest-list"]).parent
                        rejected_files.extend(folder.glob("*.avro"))
                for path in (self.path / "source_files").rglob("*.parquet"):
                    source_hashes[path] = hashlib.sha256(path.read_bytes()).hexdigest()
                raise CatalogError(409, "CommitFailedException", "Injected conversion commit conflict")
            return original(key, body)

        self.server.catalog.commit = commit
        self.run_sql(
            """
            statement ok
            CREATE TABLE lake.a_completed AS SELECT 1::INTEGER id;

            statement ok
            CREATE TABLE lake.z_failed AS SELECT 2::INTEGER id;

            statement error
            CALL ducklake_to_iceberg('lake', 'my_datalake');
            ----
            Completed tables: ["main"."a_completed"]

            query I
            SELECT id FROM my_datalake.main.a_completed;
            ----
            1

            query I
            SELECT id FROM lake.z_failed;
            ----
            2

            query I
            CALL ducklake_to_iceberg('lake', 'my_datalake', skip_tables := ['main.a_completed']);
            ----
            1

            query I
            SELECT id FROM my_datalake.main.z_failed;
            ----
            2
            """
        )
        self.assertEqual(calls["a_completed"], 1)
        self.assertEqual(calls["z_failed"], 2)
        self.assertTrue(rejected_files)
        self.assertTrue(all(not path.exists() for path in rejected_files))
        self.assertTrue(source_hashes)
        for path, expected in source_hashes.items():
            self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), expected)

    def test_unknown_commit_outcome_keeps_published_manifests(self):
        original = self.server.catalog.commit
        manifests = []

        def commit(key, body):
            result = original(key, body)
            if key[1] == "response_lost":
                manifests.extend(
                    Path(update["snapshot"]["manifest-list"])
                    for update in body["updates"]
                    if update["action"] == "add-snapshot"
                )
                raise CatalogError(503, "ServiceUnavailableException", "Response lost after successful commit")
            return result

        self.server.catalog.commit = commit
        self.run_sql(
            """
            statement ok
            CREATE TABLE lake.response_lost AS SELECT 42::INTEGER id;

            statement error
            CALL ducklake_to_iceberg('lake', 'my_datalake');
            ----
            Commit outcome may be unknown

            query I
            SELECT id FROM my_datalake.main.response_lost;
            ----
            42
            """
        )
        self.assertTrue(manifests)
        self.assertTrue(all(path.exists() for path in manifests))

    def test_empty_namespaces_and_atomic_ducklake_failure(self):
        self.run_sql(
            f"""
            statement ok
            CREATE SCHEMA lake.empty_namespace;

            query I
            CALL ducklake_to_iceberg('lake', 'my_datalake');
            ----
            0

            statement ok
            ATTACH 'ducklake:{self.path}/empty_target.ducklake' AS target
            (DATA_PATH '{self.path}/empty_target_files');

            query I
            CALL iceberg_to_ducklake('my_datalake', 'target');
            ----
            0

            query I
            SELECT count(*) FROM duckdb_schemas() WHERE database_name = 'target' AND schema_name = 'empty_namespace';
            ----
            1

            statement ok
            CREATE TABLE my_datalake.default.a_valid AS SELECT 1::INTEGER id;

            statement ok
            CREATE TABLE my_datalake.default.z_bad (id INTEGER, data VARCHAR);

            statement ok
            ALTER TABLE my_datalake.default.z_bad SET PARTITIONED BY (truncate(2, data));

            statement ok
            ATTACH 'ducklake:{self.path}/failed_target.ducklake' AS failed_target
            (DATA_PATH '{self.path}/failed_target_files');

            statement error
            CALL iceberg_to_ducklake('my_datalake', 'failed_target');
            ----
            cannot be represented in DuckLake

            query I
            SELECT max(snapshot_id) FROM __ducklake_metadata_failed_target.ducklake_snapshot;
            ----
            0

            query I
            SELECT count(*) FROM __ducklake_metadata_failed_target.ducklake_table;
            ----
            0
            """
        )


class ConversionInteropTests(ConversionTestCase):
    def test_pyiceberg_reads_exported_rows_deletes_and_history(self):
        from pyiceberg.table import StaticTable

        self.run_sql(
            """
            statement ok
            CREATE TABLE lake.interop (id INTEGER, nested STRUCT(n INTEGER, labels VARCHAR[]), txt VARCHAR);

            statement ok
            INSERT INTO lake.interop SELECT i, {'n': i * 10, 'labels': ['a', 'b']}, 'v' || i FROM range(5) r(i);

            statement ok
            ALTER TABLE lake.interop ALTER COLUMN id TYPE BIGINT;

            statement ok
            ALTER TABLE lake.interop ADD COLUMN note VARCHAR;

            statement ok
            INSERT INTO lake.interop VALUES (5, {'n': 50, 'labels': ['c']}, 'v5', 'last');

            statement ok
            DELETE FROM lake.interop WHERE id = 1;

            query I
            CALL ducklake_to_iceberg('lake', 'my_datalake', tables := ['main.interop']);
            ----
            1
            """
        )
        loaded = self.server.catalog.load((("main",), "interop"))
        table = StaticTable.from_metadata(loaded["metadata-location"])
        rows = sorted(table.scan().to_arrow().to_pylist(), key=lambda row: row["id"])
        self.assertEqual([row["id"] for row in rows], [0, 2, 3, 4, 5])
        self.assertEqual(rows[0]["nested"], {"n": 0, "labels": ["a", "b"]})
        self.assertIsNone(rows[0]["note"])
        self.assertEqual(rows[-1]["note"], "last")
        historical = table.scan(snapshot_id=2).to_arrow().to_pylist()
        self.assertEqual(sorted(row["id"] for row in historical), list(range(5)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--unittest-binary", type=Path, required=True)
    parser.add_argument("--pyiceberg", action="store_true", help="Also verify exports with installed PyIceberg/PyArrow")
    args = parser.parse_args()
    ConversionTestCase.binary = args.unittest_binary.resolve()
    if not ConversionTestCase.binary.is_file():
        parser.error("Build the unittest binary before running conversion publication tests")
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(ConversionPublicationTests)
    if args.pyiceberg:
        suite.addTests(unittest.defaultTestLoader.loadTestsFromTestCase(ConversionInteropTests))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    raise SystemExit(not result.wasSuccessful())


if __name__ == "__main__":
    main()
