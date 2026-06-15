
#include "rest_catalog/objects/fixed_type_value.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

FixedTypeValue::FixedTypeValue(string value_p) : value(std::move(value_p)) {
}

FixedTypeValue FixedTypeValue::FromJSON(yyjson_val *obj) {
	string value;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "FixedTypeValue property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return FixedTypeValue(std::move(value));
}

string FixedTypeValue::TryFromJSON(yyjson_val *obj, optional<FixedTypeValue> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FixedTypeValue FixedTypeValue::Copy() const {
	string value_tmp;
	value_tmp = value;
	return FixedTypeValue(std::move(value_tmp));
}

string FixedTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *FixedTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
