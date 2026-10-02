# Dependencies are selected in Makefile and configured in make/extension_configs.
duckdb_extension_load(iceberg
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
    LOAD_TESTS
)
