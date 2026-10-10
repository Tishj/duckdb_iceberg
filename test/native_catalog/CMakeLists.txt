add_executable(unittest_iceberg_native test_native_catalog.cpp)
target_include_directories(
  unittest_iceberg_native PRIVATE ${CMAKE_SOURCE_DIR}/third_party/catch
                                  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/testing)
set_target_properties(
  unittest_iceberg_native PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                     ${CMAKE_BINARY_DIR}/test)
# duckdb carries the configured static extension registrations. Link the Iceberg
# archive as well for direct access to the private server model used by fault
# tests.
target_link_libraries(unittest_iceberg_native iceberg_extension duckdb)
link_threads(unittest_iceberg_native "")
