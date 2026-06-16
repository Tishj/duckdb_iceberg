
#include "rest_catalog/objects/fixed_type_value.hpp"

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

FixedTypeValue::FixedTypeValue(string value_p) : value(std::move(value_p)) {
}
FixedTypeValue::FixedTypeValue(const FixedTypeValue &other) : value(other.value) {
}
FixedTypeValue::FixedTypeValue(FixedTypeValue &&other) : FixedTypeValue(static_cast<const FixedTypeValue &>(other)) {
}

string FixedTypeValue::TryFromJSON(yyjson_val *obj, optional<FixedTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("FixedTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(FixedTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FixedTypeValue FixedTypeValue::FromJSON(yyjson_val *obj) {
	optional<FixedTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

FixedTypeValue FixedTypeValue::Copy() const {
	return FixedTypeValue(*this);
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
