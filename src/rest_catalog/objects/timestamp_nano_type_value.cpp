
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
TimestampNanoTypeValue::TimestampNanoTypeValue(const TimestampNanoTypeValue &other) : value(other.value) {
}
TimestampNanoTypeValue::TimestampNanoTypeValue(TimestampNanoTypeValue &&other)
    : TimestampNanoTypeValue(static_cast<const TimestampNanoTypeValue &>(other)) {
}

optional<string> TimestampNanoTypeValue::TryFromJSON(yyjson_val *obj, optional<TimestampNanoTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "TimestampNanoTypeValue property 'value' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(obj)));
		}
		result.emplace(TimestampNanoTypeValue(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TimestampNanoTypeValue TimestampNanoTypeValue::FromJSON(yyjson_val *obj) {
	optional<TimestampNanoTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

TimestampNanoTypeValue TimestampNanoTypeValue::Copy() const {
	return TimestampNanoTypeValue(*this);
}

optional<string> TimestampNanoTypeValue::Validate() const {
	optional<string> error;
	return nullopt;
}

yyjson_mut_val *TimestampNanoTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
