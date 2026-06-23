import pytest

from duckdb_unittest import DuckDBUnittestRunner
from spark_seed import SparkSeedTable
from test_spark_read import Row


TABLE_NAME = "spark_rewrite_mor_residual_deletes"
QUALIFIED_TABLE_NAME = f"default.{TABLE_NAME}"
CATALOG_TABLE_NAME = f"my_datalake.{QUALIFIED_TABLE_NAME}"

SPARK_REWRITE_MOR_RESIDUAL_DELETES_SEED = SparkSeedTable(
    QUALIFIED_TABLE_NAME,
    f"""
    CREATE OR REPLACE TABLE {QUALIFIED_TABLE_NAME} (
        id INT,
        category STRING,
        payload STRING
    )
    USING ICEBERG
    PARTITIONED BY (category)
    TBLPROPERTIES (
        'format-version' = '2',
        'write.delete.mode' = 'merge-on-read',
        'write.update.mode' = 'merge-on-read'
    );

    INSERT INTO {QUALIFIED_TABLE_NAME} VALUES
        (1, 'compact', 'c1'),
        (2, 'compact', 'c2');

    INSERT INTO {QUALIFIED_TABLE_NAME} VALUES
        (3, 'compact', 'c3'),
        (4, 'compact', 'c4');

    INSERT INTO {QUALIFIED_TABLE_NAME} VALUES
        (5, 'compact', 'c5'),
        (6, 'compact', 'c6');

    INSERT INTO {QUALIFIED_TABLE_NAME} VALUES
        (100, 'residual', 'r100'),
        (101, 'residual', 'r101'),
        (102, 'residual', 'r102');

    UPDATE {QUALIFIED_TABLE_NAME}
    SET payload = CASE
        WHEN id = 2 THEN 'c2_u'
        WHEN id = 101 THEN 'r101_u'
        ELSE payload
    END
    WHERE id IN (2, 101);

    DELETE FROM {QUALIFIED_TABLE_NAME}
    WHERE id IN (3, 102);
    """,
)

EXPECTED_ROWS = [
    (1, "compact", "c1"),
    (2, "compact", "c2_u"),
    (4, "compact", "c4"),
    (5, "compact", "c5"),
    (6, "compact", "c6"),
    (100, "residual", "r100"),
    (101, "residual", "r101_u"),
]


class TestRewriteDataFilesSparkMorResidualDeletes:
    @pytest.mark.spark_seed_tables(SPARK_REWRITE_MOR_RESIDUAL_DELETES_SEED)
    def test_rewrite_preserves_residual_deletes_and_remains_readable_in_spark(
        self,
        catalog_connection,
        unittest_binary,
        unittest_test_config,
        print_unittest_stdin,
    ):
        with DuckDBUnittestRunner(
            unittest_binary,
            test_config=unittest_test_config,
            print_stdin=print_unittest_stdin,
        ) as test:
            test.query("I", f"select count(*) from {CATALOG_TABLE_NAME}", [(7,)])
            test.query(
                "III",
                f"""
                select id, category, payload
                from {CATALOG_TABLE_NAME}
                order by id
                """,
                EXPECTED_ROWS,
            )
            test.query(
                "I",
                f"""
                select count(*)
                from {CATALOG_TABLE_NAME}
                where id in (3, 102)
                """,
                [(0,)],
            )
            test.query(
                "I",
                f"""
                select count(*) >= 3
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'DATA' and status <> 'DELETED'
                  and file_path like '%/category=compact/%'
                """,
                [(True,)],
            )
            test.query(
                "I",
                f"""
                select count(*) < 3
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'DATA' and status <> 'DELETED'
                  and file_path like '%/category=residual/%'
                """,
                [(True,)],
            )
            test.query(
                "I",
                f"""
                select count(*) > 0
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'POSITION_DELETES' and status <> 'DELETED'
                """,
                [(True,)],
            )

            test.statement_ok(
                f"""
                call iceberg_rewrite_data_files(
                    '{CATALOG_TABLE_NAME}',
                    min_input_files => 3
                )
                """
            )

            test.query("I", f"select count(*) from {CATALOG_TABLE_NAME}", [(7,)])
            test.query(
                "III",
                f"""
                select id, category, payload
                from {CATALOG_TABLE_NAME}
                order by id
                """,
                EXPECTED_ROWS,
            )
            test.query(
                "I",
                f"""
                select count(*)
                from {CATALOG_TABLE_NAME}
                where id in (3, 102)
                """,
                [(0,)],
            )
            test.query(
                "I",
                f"""
                select count(*)
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'DATA' and status = 'ADDED'
                  and file_path not like '%/category=%'
                """,
                [(1,)],
            )
            test.query(
                "I",
                f"""
                select count(*) >= 1
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'DATA' and status <> 'DELETED'
                  and file_path like '%/category=residual/%'
                """,
                [(True,)],
            )
            test.query(
                "I",
                f"""
                select count(*) > 0
                from iceberg_metadata('{CATALOG_TABLE_NAME}')
                where content = 'POSITION_DELETES' and status <> 'DELETED'
                """,
                [(True,)],
            )

        catalog_connection.restart()
        rows = catalog_connection.con.sql(
            f"""
            select * from {QUALIFIED_TABLE_NAME} order by id
            """
        ).collect()
        assert rows == [
            Row(id=1, category="compact", payload="c1"),
            Row(id=2, category="compact", payload="c2_u"),
            Row(id=4, category="compact", payload="c4"),
            Row(id=5, category="compact", payload="c5"),
            Row(id=6, category="compact", payload="c6"),
            Row(id=100, category="residual", payload="r100"),
            Row(id=101, category="residual", payload="r101_u"),
        ]
