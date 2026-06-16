
#include "rest_catalog/objects/integer_type_value.hpp"

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

IntegerTypeValue::IntegerTypeValue(int32_t value_p) : value(std::move(value_p)) {
}

IntegerTypeValue IntegerTypeValue::FromJSON(yyjson_val *obj) {
	int32_t value;
	if (yyjson_is_int(obj)) {
		value = yyjson_get_int(obj);
	} else {
		throw InvalidInputException(
		    StringUtil::Format("IntegerTypeValue property 'value' is not of type 'integer', found '%s' instead",
		                       yyjson_get_type_desc(obj)));
	}
	return IntegerTypeValue(std::move(value));
}

string IntegerTypeValue::TryFromJSON(yyjson_val *obj, optional<IntegerTypeValue> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

IntegerTypeValue IntegerTypeValue::Copy() const {
	int32_t value_tmp;
	value_tmp = value;
	return IntegerTypeValue(std::move(value_tmp));
}

string IntegerTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *IntegerTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_int(doc, value);
}

} // namespace rest_api_objects
} // namespace duckdb
