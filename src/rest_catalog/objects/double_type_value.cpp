
#include "rest_catalog/objects/double_type_value.hpp"

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

DoubleTypeValue::DoubleTypeValue(double value_p) : value(std::move(value_p)) {
}
DoubleTypeValue::DoubleTypeValue(const DoubleTypeValue &other) : value(other.value) {
}
DoubleTypeValue::DoubleTypeValue(DoubleTypeValue &&other)
    : DoubleTypeValue(static_cast<const DoubleTypeValue &>(other)) {
}

optional<string> DoubleTypeValue::TryFromJSON(yyjson_val *obj, optional<DoubleTypeValue> &result) {
	try {
		double value;
		if (yyjson_is_num(obj)) {
			value = yyjson_get_num(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("DoubleTypeValue property 'value' is not of type 'number', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(DoubleTypeValue(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DoubleTypeValue DoubleTypeValue::FromJSON(yyjson_val *obj) {
	optional<DoubleTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

DoubleTypeValue DoubleTypeValue::Copy() const {
	return DoubleTypeValue(*this);
}

optional<string> DoubleTypeValue::Validate() const {
	optional<string> error;
	return nullopt;
}

yyjson_mut_val *DoubleTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_real(doc, value);
}

} // namespace rest_api_objects
} // namespace duckdb
