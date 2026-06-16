
#include "rest_catalog/objects/namespace.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Namespace::Namespace(vector<string> value_p) : value(std::move(value_p)) {
}

Namespace Namespace::FromJSON(yyjson_val *obj) {
	vector<string> value;
	if (yyjson_is_arr(obj)) {
		size_t idx, max;
		yyjson_val *val;
		yyjson_arr_foreach(obj, idx, max, val) {
			string tmp;
			if (yyjson_is_str(val)) {
				tmp = yyjson_get_str(val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "Namespace property 'tmp' is not of type 'string', found '%s' instead", yyjson_get_type_desc(val)));
			}
			value.emplace_back(std::move(tmp));
		}
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "Namespace property 'value' is not of type 'array', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return Namespace(std::move(value));
}

string Namespace::TryFromJSON(yyjson_val *obj, optional<Namespace> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Namespace Namespace::Copy() const {
	vector<string> value_tmp;
	value_tmp.reserve(value.size());
	for (auto &item : value) {
		value_tmp.emplace_back(item);
	}
	return Namespace(std::move(value_tmp));
}

string Namespace::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *Namespace::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *arr = yyjson_mut_arr(doc);
	for (const auto &item : value) {
		yyjson_mut_arr_append(arr, yyjson_mut_str(doc, item.c_str()));
	}
	return arr;
}

} // namespace rest_api_objects
} // namespace duckdb
