
#include "rest_catalog/objects/date_type_value.hpp"

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

DateTypeValue::DateTypeValue(string value_p) : value(std::move(value_p)) {
}
DateTypeValue::DateTypeValue(const DateTypeValue &other) : value(other.value) {
}
DateTypeValue::DateTypeValue(DateTypeValue &&other) : DateTypeValue(static_cast<const DateTypeValue &>(other)) {
}

optional<string> DateTypeValue::TryFromJSON(yyjson_val *obj, optional<DateTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("DateTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(DateTypeValue(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DateTypeValue DateTypeValue::FromJSON(yyjson_val *obj) {
	optional<DateTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

DateTypeValue DateTypeValue::Copy() const {
	return DateTypeValue(*this);
}

optional<string> DateTypeValue::Validate() const {
	optional<string> error;
	return nullopt;
}

yyjson_mut_val *DateTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
