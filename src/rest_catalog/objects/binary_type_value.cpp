
#include "rest_catalog/objects/binary_type_value.hpp"

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

BinaryTypeValue::BinaryTypeValue(string value_p) : value(std::move(value_p)) {
}
BinaryTypeValue::BinaryTypeValue(const BinaryTypeValue &other) : value(other.value) {
}
BinaryTypeValue::BinaryTypeValue(BinaryTypeValue &&other)
    : BinaryTypeValue(static_cast<const BinaryTypeValue &>(other)) {
}

string BinaryTypeValue::TryFromJSON(yyjson_val *obj, optional<BinaryTypeValue> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("BinaryTypeValue property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(BinaryTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

BinaryTypeValue BinaryTypeValue::FromJSON(yyjson_val *obj) {
	optional<BinaryTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

BinaryTypeValue BinaryTypeValue::Copy() const {
	return BinaryTypeValue(*this);
}

string BinaryTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *BinaryTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
