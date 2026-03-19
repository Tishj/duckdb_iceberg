
#pragma once

#include "duckdb.hpp"

#include "catalog/rest/api/catalog_api.hpp"
#include "common/json_wrapper.hpp"

using namespace duckdb_yyjson;
namespace duckdb {
class IcebergSchemaEntry;
class IcebergTransaction;

class ICUtils {
public:
	static yyjson_val *get_error_message(const string &api_result);
	static yyjson_doc *api_result_to_doc(const string &api_result);
};

} // namespace duckdb
