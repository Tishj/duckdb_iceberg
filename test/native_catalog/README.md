# Native REST test catalog

The native catalog implements the Python mock's REST contract inside DuckDB. It
uses an `HTTPUtil` provider and `HTTPClient`, so Iceberg still constructs requests,
serializes commits, parses responses, logs HTTP requests, and runs normal retry
logic. Requests to `http://iceberg-mock.invalid` never open a socket. Once enabled,
other HTTP origins are rejected.
HTTP filesystem sessions are rejected before HTTPFS consumes transport parameters;
warehouse reads and writes use local files.

Build first, then run the SQL suite serially:

```sh
make reldebug
make test_mock_native_reldebug
```

To focus the catalog suite:

```sh
make test_mock_native_reldebug \
  MOCK_NATIVE_TEST_FILTER='*test/sql/local/catalog_test_config_setup/catalog_agnostic/transactions/*'
```

The target also runs the native protocol and lifecycle SQL tests. It uses
`test/configs/mock_native.json` directly and does not change
`.catalogs/.active_catalog` or start/stop services. The config inherits the Python
mock's capabilities and explicit exclusions through a relative `extends` entry.
Unlike the Python config, it does not skip unexpected unsupported errors.

The standalone API tests exercise independent database lifetimes and publication
failure after completing the first file of a multi-table commit:

```sh
cmake --build build/reldebug --target unittest_iceberg_native
./build/reldebug/test/unittest_iceberg_native
```

## SQL test interfaces

These interfaces are compiled into Iceberg but have no effect until invoked.
They are test helpers, not a supported persistent catalog:

```sql
CALL iceberg_test_catalog_init('/tmp/my-native-catalog-test');

ATTACH '' AS my_datalake (
    TYPE ICEBERG,
    URI 'http://iceberg-mock.invalid',
    AUTHORIZATION_TYPE 'none',
    ACCESS_DELEGATION_MODE 'none',
    STAGE_CREATE_TABLES true
);

SELECT status, body
FROM iceberg_test_catalog_request('GET', '/v1/config', NULL);
```

Initialization loads HTTPFS before installing the native provider. Repeating it
with the same root is a no-op; supplying another root is an error. The root must
be a local directory. Each database creates a unique child directory containing
its warehouse and a bounded-payload `requests.jsonl` journal. Metadata files
remain complete and immutable. The test runner owns cleanup.

All connections in one `DatabaseInstance` share catalog state, even after the
initializing connection closes. Different databases are isolated, including
when given the same root. Reopening a database starts an empty catalog; old files
are not reloaded. Do not change HTTP providers while using this test catalog.

The request helper accepts a method, an absolute REST path beginning with `/`,
and a JSON body string or SQL NULL. It returns `status INTEGER, body VARCHAR`.
GET, POST, HEAD, DELETE, PUT, and OPTIONS are accepted; unsupported routes return
the same REST error envelope as the server. Mutations occur during execution,
not during binding or EXPLAIN.

Supported behavior and exclusions follow the
[Python mock documentation](../../scripts/mock_rest_catalog/README.md).
The C++ model validates requirements against published metadata and applies
updates in request order. It completes every file in a batch before atomically
publishing table pointers. Failed batches may leave unreferenced files for
diagnosis but do not publish partial changes.

Publication crosses the request's starting millisecond when necessary. This keeps
metadata timestamps distinguishable from transactions that began before the
request, despite the in-process transport's speed. The clock remains real wall
time; commits are never assigned future timestamps.

## Core CI

An already-built core binary with this Iceberg revision and matching dependencies
can run the same SQL suite from the core checkout:

```sh
/path/to/core/build/reldebug/test/unittest \
  --test-config /path/to/iceberg/test/configs/mock_native.json \
  '*test/sql/local/catalog_test_config_setup/catalog_agnostic/*' \
  'exclude:*.test_slow'
```

The leading `*` handles core's absolute extension test names. Config inheritance
is relative to the config file, independent of the working directory. Apply this
config only to the selected Iceberg tests.

The companion core workflow `IcebergNativeCatalog.yml` accepts a full immutable
Iceberg commit SHA. It checks out that revision, selects its extension config
through `DUCKDB_ICEBERG_NATIVE_TEST_SOURCE_DIR`, and builds matching dependencies.
Both API and SQL tests then run in an empty Linux network namespace. This avoids
changing the ordinary core Iceberg pin before the native implementation has a
published revision; the permanent pin update must use that published commit.

Extension CI separately runs the native SQL suite without networking and retains
the Python mock job for real HTTP coverage. Real catalogs remain the source of
interoperability coverage.
