
#include "rest_catalog/objects/decimal_type_value.hpp"

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

DecimalTypeValue::DecimalTypeValue(string value_p) : value(std::move(value_p)) {
}
DecimalTypeValue::DecimalTypeValue(const DecimalTypeValue &other) : value(other.value) {
}
DecimalTypeValue::DecimalTypeValue(DecimalTypeValue &&other)
    : DecimalTypeValue(static_cast<const DecimalTypeValue &>(other)) {
}

optional<string> DecimalTypeValue::TryFromJSON(yyjson_val *obj, optional<DecimalTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("DecimalTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(DecimalTypeValue(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DecimalTypeValue DecimalTypeValue::FromJSON(yyjson_val *obj) {
	optional<DecimalTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

DecimalTypeValue DecimalTypeValue::Copy() const {
	return DecimalTypeValue(*this);
}

optional<string> DecimalTypeValue::Validate() const {
	optional<string> error;
	return nullopt;
}

yyjson_mut_val *DecimalTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
