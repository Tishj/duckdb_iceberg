add_library(
  iceberg_function_ducklake OBJECT
  catalog_conversion.cpp ducklake_to_iceberg.cpp iceberg_to_ducklake.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:iceberg_function_ducklake>
    PARENT_SCOPE)
