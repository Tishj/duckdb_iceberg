# Converting between Iceberg and DuckLake

`iceberg_to_ducklake` and `ducklake_to_iceberg` convert catalog metadata while
reusing existing data and delete files. They do not copy, rewrite, or relocate
those files. Iceberg export creates new manifests and manifest lists in storage
allocated by the destination REST catalog.

Conversion includes currently existing tables, their retained data snapshots,
and representable schema changes. It validates all selected tables before
publishing destination tables. A feature that prevents faithful file reuse causes
an error; the converter does not silently omit the affected table or its history.

## Setup and calls

Load both extensions and attach the source and destination catalogs. The examples
below assume `ice` is an attached Iceberg REST catalog.

```sql
LOAD iceberg;
LOAD ducklake;

ATTACH 'ducklake:/absolute/path/lake.ducklake' AS lake
    (DATA_PATH '/absolute/path/lake-files', DATA_INLINING_ROW_LIMIT 0);

CALL iceberg_to_ducklake(
    'ice', 'lake',
    tables := ['analytics.orders', 'analytics.customers']
);
```

The destination DuckLake must be newly initialized: snapshot 0 and no tables.
Existing empty tables still make it a nonempty destination.

For the reverse direction, attach an Iceberg REST destination with staged
creation enabled:

```sql
-- ice_destination is an attached writable REST catalog with
-- STAGE_CREATE_TABLES true.
CALL ducklake_to_iceberg(
    'lake', 'ice_destination',
    tables := ['analytics.orders']
);
```

Unrelated Iceberg tables may already exist. Every selected destination table name
must be unused; conversion never replaces an existing table.

Both calls return one `count BIGINT` row, counting converted tables. They require
autocommit, so finish an explicit transaction before calling them. `EXPLAIN` does
not execute conversion.

| Argument | Meaning |
| --- | --- |
| First positional argument | Attached source catalog name. |
| Second positional argument | Attached destination catalog name. |
| `tables` | Optional list of source-relative, schema-qualified names. Omission selects all visible current tables; `[]` selects none. |
| `skip_tables` | Exclusions applied after selection. A bare name excludes that name in every schema; a qualified name excludes only that table. |
| `metadata_catalog`, `metadata_schema` | Optional assertions identifying the DuckLake metadata attachment. They must match its attachment configuration. |

Use SQL identifier quoting inside table-name strings when necessary:

```sql
CALL ducklake_to_iceberg(
    'lake', 'ice_destination',
    tables := ['"sales reports"."customer''s orders"']
);
```

Unknown explicit selections and NULL arguments/list entries are errors. Without
`tables`, empty namespaces are copied too. With an explicit table selection, the
converter creates the namespaces needed by those tables.

### DuckLake versions and metadata backends

The supported metadata backend is DuckDB. PostgreSQL, SQLite, and other metadata
backends are rejected.

The build pins DuckLake commit
[`94092e61a164cdca7ea838bd0c8a9f873fd0f17e`](https://github.com/duckdb/ducklake/commit/94092e61a164cdca7ea838bd0c8a9f873fd0f17e).
The converters recognize metadata versions `1.0` and the `1.1-dev1` layout at
that commit. Required metadata columns and types are checked before writing.
Other versions or changed layouts require a converter update.

This build applies two compatibility fixes in a generated source copy: correct
Iceberg position-delete field IDs, and handling of variant container statistics.
The fetched DuckLake checkout remains unchanged. Existing files written with
incorrect field IDs are not repaired.

Conversion does not upgrade metadata. Migrate an older DuckLake separately using
DuckLake's supported migration procedure before converting it.

Custom metadata names must be explicit attachment options:

```sql
ATTACH 'ducklake:/absolute/path/catalog.ducklake' AS lake
    (DATA_PATH '/absolute/path/data',
     METADATA_CATALOG 'lake metadata',
     METADATA_SCHEMA 'lake schema');

CALL ducklake_to_iceberg(
    'lake', 'ice_destination',
    metadata_catalog := 'lake metadata',
    metadata_schema := 'lake schema'
);
```

If a secret supplies custom metadata names, also specify those names explicitly
in `ATTACH`. Conversion does not guess which of several metadata databases or
schemas belongs to an attachment.

## What is preserved

- Current rows and supported DuckDB-visible column types, including nested
  structs, lists, and maps.
- Retained table states, column names and field IDs, supported type widening,
  nullability, and literal defaults.
- Empty tables and schema-only changes. The forward converter reads retained
  Iceberg metadata-log files to recover changes that created no data snapshot,
  including a final ALTER after the last insert.
- Compatible partition specifications and per-file partition values, including
  files written under older specifications.
- Compatible delete files and their applicability at each retained state.
- Existing file contents and locations. Relative metadata paths are resolved
  against the source path hierarchy before export.

Statistics need not be identical. The DuckLake import writes correct table row
counts, file sizes, and allocation counters, and leaves optional column statistics
unknown rather than inventing bounds or treating truncated bounds as exact.
The result remains writable.

The reverse converter chooses Iceberg v2 when sufficient and v3 when supported
types, defaults, or deletion vectors require it. A catalog can still refuse the
requested format or staged schema. In particular, a catalog that reassigns field
IDs during staging is incompatible with file reuse.

### History and time travel

DuckLake snapshots are catalog-wide; Iceberg snapshots are table-specific.
Consequently, snapshot numbers and the number of metadata-only events need not
match between formats.

DuckLake export uses the retained DuckLake snapshot IDs for the corresponding
Iceberg table states. Its snapshot summaries contain `ducklake.snapshot-id`,
`ducklake.timestamp-us`, and `ducklake.table-uuid`.

Iceberg import assigns ordered DuckLake snapshot IDs. Each imported table event's
`commit_extra_info` records the source table UUID, source snapshot ID and
timestamp, and whether the event was synthesized from metadata. Synthetic events
have source snapshot ID 0. Equal timestamps do not merge distinct snapshots.

Iceberg timestamps have millisecond precision. Original DuckLake microsecond
timestamps remain in provenance, but timestamp-based Iceberg queries cannot
distinguish events inside one millisecond; use snapshot IDs for those states.

Only retained history can be converted. Expired snapshots are not recreated, and
retained snapshots or metadata-log entries whose files have been removed cause
an error. Creation times unavailable from the source are represented by
conversion events.

Dropped tables, historical catalog names, views, macros, branches/tags,
cross-table transactional history, arbitrary catalog/table properties, sort
settings, and cross-format system row identities are not preservation
guarantees. Branching or rollback histories that cannot be represented by the
linear forward import are rejected.

## Compatibility gaps and remedies

These checks apply to retained history, not only the latest state. A clean
current table can still fail because an earlier retained state is incompatible.

| Feature or layout | Behavior and remedy |
| --- | --- |
| Non-Parquet data files | Rejected. Rewrite the source data to compatible Parquet before conversion. |
| Unsupported types | Unsigned integers, 128-bit integers, intervals, time-with-zone, nanosecond time, geometry/CRS types, aliases/collations, and other types without a supported mapping are rejected. Cast and rewrite explicitly if a different type is acceptable. |
| Compatible v3 types | Variant and nanosecond timestamp representations are supported; these require a v3-capable Iceberg destination. Variant's internal Parquet fields do not require independent Iceberg field IDs. |
| Expression defaults | Only literal defaults are converted, including a quoted literal whose text is `NULL`. Expressions such as `now()` are rejected. Replace the default explicitly if changing its behavior is acceptable. |
| Incompatible schema evolution | Narrowing, incompatible type changes, or moving a field ID to a different parent are rejected. Integer-to-bigint, float-to-double, and decimal precision widening with unchanged scale are supported. |
| Missing field IDs or per-file name mappings | Rejected. Existing Parquet mappings that need changes to physical field IDs cannot be repaired by relabeling metadata. |
| Embedded row lineage/sequence columns | Rejected when the physical columns need remapping to destination identities. Rewriting the affected data files is required. |
| Iceberg identifier fields | Rejected because their constraint metadata has no supported DuckLake mapping. |
| Partition transforms | Identity and compatible bucket transforms are supported. Iceberg year/month/day/hour map to DuckLake `epoch_year`/`epoch_month`/`epoch_day`/`epoch_hour`. DuckLake calendar year is translated by subtracting 1970 from partition values. Calendar month/day/hour are not epoch transforms and are rejected. Iceberg truncate and void transforms are rejected. |
| Nested namespaces | Require DuckLake metadata `1.1-dev1`. Iceberg uses flattened quoted schema names such as `"outer.inner"` in SQL. Literal dots inside a namespace component are not distinguishable by this Iceberg catalog implementation and are rejected on export. Catalog namespace visibility still follows the attachment's options. |
| Case-colliding names | Tables/namespaces that differ only by case cannot both be imported into DuckLake. Case-colliding column names are also rejected. Rename the conflicting objects explicitly before conversion. |
| Equality deletes | Rejected. Materialize their effect by rewriting source data with a compatible writer. |
| Multiple active delete files for one data file | Rejected when their combined effect requires merging delete contents. Effective Iceberg vectors supersede remnant position deletes. |
| Position deletes spanning several data files | Rejected. The forward conversion requires a proven single referenced data file. Missing or differing file-path bounds also prevent that proof. |
| Relative paths inside position-delete files | Rejected. Updating catalog paths cannot update strings embedded inside an existing Parquet delete file. Use an absolute `DATA_PATH` when creating DuckLake files, or rewrite existing deletes. |
| Nonstandard position-delete field IDs | Rejected before publication. Iceberg requires `file_path=2147483546` and `pos=2147483545`. Older DuckLake files can use the virtual filename/ordinal IDs instead and need rewriting by a compatible writer. This build fixes the writer for newly created files. |
| Bare deletion-vector blobs | DuckLake can read them, so the forward converter can import compatible legacy blobs. Reverse conversion rejects them: Iceberg requires a Puffin container/footer, which cannot be added without rewriting the file. |
| Puffin containers | Forward conversion supports one compatible vector blob per container. It rejects multiple blobs because DuckLake metadata cannot select an arbitrary Iceberg offset. Reverse conversion supports compatible DuckLake cumulative containers by selecting each retained snapshot's blob offset and length. Compressed/unsupported layouts, invalid ranges, checksums, or cardinalities fail validation. |
| Inlined rows or deletes | Rejected when still represented by inline metadata. They have no independently reusable Iceberg data/delete file. |
| Partial files with per-row snapshot filtering | Marked partial data files and Parquet deletes are rejected. Flushing or compacting the latest state alone does not prove historical compatibility. Compatible cumulative Puffin containers use the per-snapshot handling described above. |
| Encrypted files | Rejected if the encryption representation cannot be reused. Access to metadata alone is insufficient. |

Rewriting or expiring history is a separate source-maintenance operation; the
converter never does either automatically. Expiration intentionally discards
history, and flushing can produce partial files that remain incompatible.
Inspect the resulting layout before retrying.

## Publication, recovery, and shared storage

Iceberg-to-DuckLake publication is one DuckDB metadata transaction. A failure
rolls back the import.

DuckLake-to-Iceberg publication is atomic **per table**, using staged creation and
one REST commit containing that table's history. Namespace creation is separate.
If a later table fails, earlier completed tables remain usable. The error lists
completed tables; retry with those tables excluded:

```sql
CALL ducklake_to_iceberg(
    'lake', 'ice_destination',
    skip_tables := ['analytics.already_completed']
);
```

A timeout or server error can leave the commit outcome unknown. Inspect the
destination before retrying. Potentially committed manifests are retained in
that case. Cleanup after a definite failure affects only newly generated
metadata files and is best effort.

Both catalogs reference the same data and delete files after conversion.
Keep those files accessible to every intended reader and preserve the required
storage credentials. A path on the conversion host is not automatically
accessible to remote Spark workers.

Coordinate snapshot expiration, orphan-file cleanup, table purges, and storage
lifecycle policies across both catalogs. One catalog does not know which files
the other still references. Conversion transfers neither file ownership nor
credentials, and it cannot protect retained files against concurrent external
cleanup.

## Development validation

The normal catalog-agnostic suite includes small conversion tests; the larger
round trip is `test/sql/iceberg_to_ducklake.test_slow`. Run them serially with the
existing active catalog configuration.

Publication-failure tests use private temporary mock servers and the built SQL
runner, without changing the active catalog marker:

```sh
python3 -m scripts.mock_rest_catalog.test_conversion \
    --unittest-binary build/reldebug/test/unittest
```

Add `--pyiceberg` to also read exported current/historical data and position deletes
with the PyIceberg and PyArrow versions pinned in `scripts/requirements.txt`. The
mock CI job runs this independent-reader check.
