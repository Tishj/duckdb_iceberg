#include "common/json_wrapper.hpp"

namespace duckdb {

string JsonToString(json_unique_ptr<yyjson_mut_doc> doc) {
	auto root_object = yyjson_mut_doc_get_root(doc.get());

	//! Write the result to a string
	auto data = yyjson_mut_val_write_opts(root_object, YYJSON_WRITE_ALLOW_INF_AND_NAN, nullptr, nullptr, nullptr);
	if (!data) {
		throw InvalidInputException("Could not serialize the JSON to string, yyjson failed");
	}
	auto res = string(data);
	free(data);
	return res;
}

} // namespace duckdb
