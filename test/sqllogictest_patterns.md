# Reusable sqllogictest patterns

Use includes for repeated setup or assertions, SQL variables for values discovered
at runtime, and tester variables for substituting SQL text. Keep the behavior
under test and its expected result visible at the call site.

The examples below are implemented in existing tests. They are deliberately
small migrations, not a requirement to convert every test to the same shape.

## Variable syntax and execution timing

These are different constructs:

```text
# SQL statement: evaluates a scalar expression or query on this connection.
statement ok
SET VARIABLE snapshot_ids = (SELECT list(snapshot_id ORDER BY sequence_number) FROM iceberg_snapshots(my_datalake.default.example));

# Tester directive: captures a SQL variable as text while parsing.
set variable SNAPSHOT_IDS <variable:snapshot_ids>

# Tester loop: expands the SQL list when the loop executes.
foreach snapshot_id <variable:snapshot_ids>

query I
SELECT count(*) FROM my_datalake.default.example AT (VERSION => {snapshot_id});
----
42

endloop
```

A tester directive uses `set variable NAME value`, without `=` or `statement ok`.
SQL uses `SET VARIABLE name = expression` following `statement ok`.
`<variable:connection_name:variable_name>` reads from a named connection.

Use uppercase names for explicit tester parameters, lowercase for SQL variables
and loop iterators. Substitution is textual: quote SQL string literals and
identifiers appropriately. Do not interpolate arbitrary strings without escaping
them. Numeric IDs and controlled fixture paths are convenient loop inputs.

The loop input is cast to a list of strings. Use `list(value ORDER BY ...)` for
stable ordering, and `DISTINCT` only when duplicates are irrelevant to the test.
A NULL list fails expansion; an empty list does not exercise the loop body. Assert
expected cardinality or membership before discovery-driven loops. Do not silently
coalesce unexpected NULL lists to empty lists.

`foreach ... <variable:...>` resolves its list each time that loop is entered.
It can therefore use a SQL variable assigned by the enclosing loop. A standalone
`set variable NAME <variable:...>` is parsed immediately and cannot capture a
value that a deferred loop body has not produced yet.

## 1. Compose catalog setup from parameterized sqllogictest includes

[fixture.json](configs/fixture.json) selects
[fixture_init.inc](configs/fixture_init.inc) through `init_sqllogic`. Setup uses
ordinary tester directives and `statement ok` blocks, so configs and custom-setup
tests can share the same implementation.

The fixture setup is split into three reusable parts:

- [defaults.inc](sql/include/fixture/defaults.inc) initializes tester parameters
  for the warehouse, catalog alias, REST credentials, URI, and extra attach
  options. Include it once before overriding parameters.
- [storage_secret.inc](sql/include/fixture/storage_secret.inc) creates storage
  credentials once per database, independently of attachment.
- [attach.inc](sql/include/fixture/attach.inc) attaches using the current
  parameters. It does not reset defaults, recreate secrets, or create schemas.

The config wrapper composes these and creates the default schema.
[table_resolution.test](sql/local/catalog_custom_setup/fixture/attach_options/table_resolution.test)
uses the same components directly, without the config's automatic attachment.
It reuses the attach include with default options, then overrides it for eager
resolution:

```text
statement ok
SET VARIABLE fixture_attach_options = ', TABLE_RESOLUTION ''eager''';

set variable FIXTURE_ATTACH_OPTIONS <variable:fixture_attach_options>

include test/sql/include/fixture/attach.inc
```

Detach the current alias before attaching again. Other overrides, such as
`set variable FIXTURE_CATALOG another_catalog`, follow the same pattern.
`FIXTURE_WAREHOUSE` is a complete SQL string literal; `FIXTURE_CATALOG` is an
identifier. Extra options are raw SQL with a leading comma, or an empty string.
Use a SQL variable and the scalar bridge for fragments with spaces: tester
`set variable` values must be a single token. Parameters persist across includes,
so explicitly reset options when changing scenarios.

`init_sqllogic` runs once before the test body. Named connections in that database
share its attachment and storage secrets; they do not need to repeat setup.
Unlike `on_init` / `init_script`, this hook does not rerun when a database is
loaded or restarted. Tests that explicitly create a fresh database must include
setup again. Keep connection-local initialization in `on_new_connection` when
needed. Do not assume this config supports automatic forced reloads without
additional lifecycle support.

Keep catalog markers and reasoned skip policies in JSON. The separate
`fixture_duckdb_tests.json` configuration remains independent. Run custom-setup
tests with their availability marker and without `fixture.json`, so they own the
attachment lifecycle. Run from the repository root, as with existing commands.

## 2. Give a parameterized include an explicit contract

[identity_integer.test](sql/local/catalog_test_config_setup/catalog_agnostic/sorting/identity/identity_integer.test)
uses [latest_added_data_file.inc](sql/include/latest_added_data_file.inc):

```text
set variable TABLE my_datalake.default.sort_identity_integer

include test/sql/include/latest_added_data_file.inc
```

The helper takes a fully qualified table identifier, asserts exactly one ADDED
data file in the latest manifest sequence, and sets SQL variables
`latest_manifest_sequence` and `file_path`. The caller retains the expected
physical row order and table cleanup.

Document required parameters, overwritten SQL variables, and transaction
ownership in the include. Includes share caller state; they are not functions
with local variables. Use repository-relative paths and `.inc` files so helpers
are not collected as standalone tests. Include filenames are literal in the
current runner; parameterize their contents. A loop iterator can also supply a
placeholder in an included statement.

## 3. Capture a scalar when SQL needs a literal

[test_time_travel_future_timestamp.test](sql/local/catalog_test_config_setup/catalog_agnostic/test_time_travel_future_timestamp.test)
replaces a query-string template and `query(...replace(...))` with:

```text
set variable FIRST_TIMESTAMP <variable:first_timestamp>

query I
SELECT i FROM my_datalake.default.time_travel_future_timestamp
AT (TIMESTAMP => TIMESTAMP '{FIRST_TIMESTAMP}') ORDER BY i;
----
1
2
3
```

Keep `getvariable()` when SQL accepts the expression directly. Use the bridge
when literal substitution makes an assertion simpler, and capture the value
after its SQL assignment has executed.

## 4. Iterate over snapshot IDs

[test_time_travel_future_timestamp.test](sql/local/catalog_test_config_setup/catalog_agnostic/test_time_travel_future_timestamp.test)
asserts that its two committed snapshots exist, then iterates over their IDs
using `<variable:snapshot_ids>`. The first version must contain exactly
`[1, 2, 3]`, and the second exactly `[1, 2, 3, 4]`. The rolled-back insert must
not introduce another snapshot.

The expected rows are authored independently of the historical reads. Comparing
ordered lists checks both values and multiplicity. The same file demonstrates
the scalar timestamp bridge above, while the nested metadata example below
shows how to discover a new inner list during each iteration.

## 5. Iterate over manifests and aggregate observations

[test_merge_append_txn_schema_change.test](sql/local/catalog_test_config_setup/catalog_agnostic/insert/test_merge_append_txn_schema_change.test)
replaces three `OFFSET` lookups and a repeated `UNION ALL` with:

```text
statement ok
set variable txn_manifests = (
    select list(manifest_path order by manifest_path)
    from read_avro(getvariable('latest_txn_manifest_list'))
);

query I
select len(getvariable('txn_manifests'));
----
3

statement ok
create or replace temp table manifest_schemas (schema_id VARCHAR);

foreach manifest_path <variable:txn_manifests>

statement ok
insert into manifest_schemas
select value from avro_metadata('{manifest_path}') where key = 'schema-id';

endloop
```

The final distribution assertions remain explicit: two schema-0 manifests and
one schema-1 manifest at the first checkpoint, then one and two respectively.
Reset the temporary observations table between checkpoints. Keep the surrounding
transaction open while resolving and reading files so vended credentials stay
in scope.

## 6. Validate every produced data file

[scan_tasks.test](sql/local/catalog_test_config_setup/catalog_agnostic/scan_planning/scan_tasks.test)
collects distinct file paths from its saved scan tasks and asserts exactly three.
A loop reads each Parquet file and checks 3000 rows and 3000 distinct IDs. This
checks every physical file, avoiding an arbitrary `LIMIT 1` sample.

Only apply uniform per-file expectations when the setup guarantees them. This
test writes 9000 distinct IDs into three equally sized partitions. For uneven
files, store independently known expectations per file or compare with the
appropriate logical subset. Preserve existing single-file cardinality assertions
in tests whose purpose requires exactly one file.

## 7. Traverse metadata with nested loops

[test_manifest_compression.test](sql/local/catalog_test_config_setup/catalog_agnostic/insert/test_manifest_compression.test)
collects manifest-list paths for its two snapshots. For each path, SQL builds a
new list of manifest paths, and an inner loop checks the codec header and that
`read_avro` can read the manifest.

```text
foreach manifest_list <variable:manifest_lists>

statement ok
set variable snapshot_manifests = (
    select list(manifest_path order by manifest_path)
    from read_avro('{manifest_list}')
);

# The real test asserts a nonempty list here.
foreach manifest_path <variable:snapshot_manifests>

# Check this manifest.

endloop

endloop
```

The test now checks historical manifests as well as current ones. A manifest
shared by two snapshots may be checked twice: the traversal validates each
snapshot's reachable metadata. Assert the expected outer size and nonempty inner
lists so missing metadata cannot reduce coverage unnoticed.

## 8. Replay each discovered partition

[scan_tasks.test](sql/local/catalog_test_config_setup/catalog_agnostic/scan_planning/scan_tasks.test)
also derives distinct partition values from `saved_tasks`, checks that they are
exactly `[0, 1, 2]`, and replays the corresponding task subset for each value.

Each subset must contain 3000 rows. Bidirectional `EXCEPT ALL` against the
corresponding rows of the saved logical result checks values and multiplicity,
including nested columns. This extends the old check of partition 1 to every
partition without duplicating the replay query three times. The full-task replay
assertions remain in place.

## 9. Drive a test matrix from explicit SQL cases

[test_metrics_mode_configuration.test](sql/local/catalog_test_config_setup/catalog_agnostic/insert/test_metrics_mode_configuration.test)
defines `bound_cases(field_id, expected_bytes)` with `(1, 8)`, `(2, 16)`, and
`(5, 4)` for default truncation, full bounds, and per-column truncation.
A SQL list supplies the field IDs to a loop that checks lower and upper bounds.

Expected sizes are authored in the case table. Comparing a list of actual bound
lengths against a singleton expected list also asserts exactly one bound for
each field. The separate checks for absent bounds and value counts remain
explicit, since they test a different behavior.

This pattern works for richer cases too: keep input expressions, type names, or
expected values in SQL rows, and iterate over simple case IDs. Prefer a literal
`foreach` for a small list with no associated case data. Do not derive expected
values from the output under test.

## Validation

Run changed tests serially using the active catalog configuration. From the
repository root, resolve that configuration through Bash:

```bash
source scripts/catalog_test_config.sh
TEST_CONFIG="$(active_catalog_test_config)"
./build/debug/test/unittest --test-config "$TEST_CONFIG" \
  test/sql/local/catalog_test_config_setup/catalog_agnostic/insert/test_merge_append_txn_schema_change.test
```

These examples create their own tables; they do not need generated Spark data.
Other catalogs' existing skip policies still apply.
After editing JSON configs, run `jq empty test/configs/*.json`. Do not start or
regenerate a catalog just to inspect this guide.
