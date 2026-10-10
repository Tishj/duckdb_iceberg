add_library(iceberg_testing OBJECT catalog_json.cpp catalog_model.cpp
                                   catalog_router.cpp catalog_transport.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:iceberg_testing>
    PARENT_SCOPE)
