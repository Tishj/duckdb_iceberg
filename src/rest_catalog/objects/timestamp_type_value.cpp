
#include "rest_catalog/objects/timestamp_type_value.hpp"

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

TimestampTypeValue::TimestampTypeValue(string value_p) : value(std::move(value_p)) {
}

string TimestampTypeValue::TryFromJSON(yyjson_val *obj, optional<TimestampTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("TimestampTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(TimestampTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TimestampTypeValue TimestampTypeValue::FromJSON(yyjson_val *obj) {
	optional<TimestampTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

TimestampTypeValue TimestampTypeValue::Copy() const {
	string value_tmp;
	value_tmp = value;
	return TimestampTypeValue(std::move(value_tmp));
}

string TimestampTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *TimestampTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
