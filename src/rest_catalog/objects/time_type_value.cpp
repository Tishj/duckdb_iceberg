
#include "rest_catalog/objects/time_type_value.hpp"

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

TimeTypeValue::TimeTypeValue(string value_p) : value(std::move(value_p)) {
}
TimeTypeValue::TimeTypeValue(const TimeTypeValue &other) : value(other.value) {
}
TimeTypeValue::TimeTypeValue(TimeTypeValue &&other) : TimeTypeValue(static_cast<const TimeTypeValue &>(other)) {
}

string TimeTypeValue::TryFromJSON(yyjson_val *obj, optional<TimeTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("TimeTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(TimeTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TimeTypeValue TimeTypeValue::FromJSON(yyjson_val *obj) {
	optional<TimeTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

TimeTypeValue TimeTypeValue::Copy() const {
	return TimeTypeValue(*this);
}

string TimeTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *TimeTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
