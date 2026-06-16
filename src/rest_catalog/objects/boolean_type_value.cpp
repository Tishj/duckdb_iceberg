
#include "rest_catalog/objects/boolean_type_value.hpp"

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

BooleanTypeValue::BooleanTypeValue(bool value_p) : value(std::move(value_p)) {
}
BooleanTypeValue::BooleanTypeValue(const BooleanTypeValue &other) : value(other.value) {
}
BooleanTypeValue::BooleanTypeValue(BooleanTypeValue &&other)
    : BooleanTypeValue(static_cast<const BooleanTypeValue &>(other)) {
}

string BooleanTypeValue::TryFromJSON(yyjson_val *obj, optional<BooleanTypeValue> &result) {
	try {
		bool value;
		if (yyjson_is_bool(obj)) {
			value = yyjson_get_bool(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("BooleanTypeValue property 'value' is not of type 'boolean', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(BooleanTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

BooleanTypeValue BooleanTypeValue::FromJSON(yyjson_val *obj) {
	optional<BooleanTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

BooleanTypeValue BooleanTypeValue::Copy() const {
	return BooleanTypeValue(*this);
}

string BooleanTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *BooleanTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_bool(doc, value);
}

} // namespace rest_api_objects
} // namespace duckdb
