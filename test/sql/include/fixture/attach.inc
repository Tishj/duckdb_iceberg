# Inputs: FIXTURE_WAREHOUSE, FIXTURE_CATALOG, FIXTURE_CLIENT_ID,
# FIXTURE_CLIENT_SECRET, FIXTURE_URI, FIXTURE_ATTACH_OPTIONS (see defaults.inc).
# Requires iceberg and the storage secret. Does not reset parameters, create
# schemas, or manage transactions. Detach the alias before reusing this include.
statement ok
ATTACH {FIXTURE_WAREHOUSE} AS {FIXTURE_CATALOG} (
    TYPE ICEBERG,
    CLIENT_ID '{FIXTURE_CLIENT_ID}',
    CLIENT_SECRET '{FIXTURE_CLIENT_SECRET}',
    URI '{FIXTURE_URI}'
    {FIXTURE_ATTACH_OPTIONS}
);
