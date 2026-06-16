
#include "rest_catalog/objects/timestamp_nano_type_value.hpp"

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

TimestampNanoTypeValue::TimestampNanoTypeValue(string value_p) : value(std::move(value_p)) {
}

TimestampNanoTypeValue TimestampNanoTypeValue::FromJSON(yyjson_val *obj) {
	string value;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		throw InvalidInputException(
		    StringUtil::Format("TimestampNanoTypeValue property 'value' is not of type 'string', found '%s' instead",
		                       yyjson_get_type_desc(obj)));
	}
	return TimestampNanoTypeValue(std::move(value));
}

string TimestampNanoTypeValue::TryFromJSON(yyjson_val *obj, optional<TimestampNanoTypeValue> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TimestampNanoTypeValue TimestampNanoTypeValue::Copy() const {
	string value_tmp;
	value_tmp = value;
	return TimestampNanoTypeValue(std::move(value_tmp));
}

string TimestampNanoTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *TimestampNanoTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
