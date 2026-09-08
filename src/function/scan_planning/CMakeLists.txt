add_library(iceberg_function_scan_planning OBJECT iceberg_scan_plan.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:iceberg_function_scan_planning>
    PARENT_SCOPE)
