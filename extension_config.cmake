# This file is included by DuckDB's build system. It specifies which extension to load
if (NOT EMSCRIPTEN)
  duckdb_extension_load(avro
  LOAD_TESTS
  GIT_URL https://github.com/duckdb/duckdb-avro
  GIT_TAG fa09aa71a716703cbd865a4f3cea17224202cde1
  SUBMODULES "third_party/avro-c"
  APPLY_PATCHES
)
  duckdb_extension_statically_link(avro)
endif()

# Extension from this repo
duckdb_extension_load(parquet)
duckdb_extension_load(json)
duckdb_extension_load(iceberg
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
    LOAD_TESTS
)
# Static registration order is also database startup load order. Avro above and
# Parquet here must precede Iceberg, so fresh databases (including CSV result readers)
# do not try to auto-install unsigned copies from the build's extension repository.
duckdb_extension_statically_link(parquet json iceberg)

if (NOT EMSCRIPTEN)
  duckdb_extension_load(tpch)
  duckdb_extension_load(icu)
  duckdb_extension_statically_link(tpch icu httpfs)
  duckdb_extension_load(ducklake
        LOAD_TESTS
        GIT_URL https://github.com/duckdb/ducklake
        GIT_TAG 94092e61a164cdca7ea838bd0c8a9f873fd0f17e
  )
  if (DUCKDB_EXTENSION_DUCKLAKE_SHOULD_BUILD)
    if (NOT DUCKDB_EXTENSION_DUCKLAKE_PREBUILT_PATH)
      include("${CMAKE_CURRENT_LIST_DIR}/make/ducklake_compat.cmake")
    endif()
    duckdb_extension_statically_link(ducklake)
  endif()

  if (NOT MINGW)
    duckdb_extension_load(aws
            LOAD_TESTS
            GIT_URL https://github.com/duckdb/duckdb-aws
            GIT_TAG 7eaa663835aa5c627cb2b6f50ffcc5487ad973f3
            APPLY_PATCHES
    )
    duckdb_extension_statically_link(aws)
  endif()
endif()
