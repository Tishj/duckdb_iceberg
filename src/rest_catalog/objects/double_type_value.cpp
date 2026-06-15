
#include "rest_catalog/objects/double_type_value.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

DoubleTypeValue::DoubleTypeValue(double value_p) : value(std::move(value_p)) {
}

DoubleTypeValue DoubleTypeValue::FromJSON(yyjson_val *obj) {
	double value;
	if (yyjson_is_num(obj)) {
		value = yyjson_get_num(obj);
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "DoubleTypeValue property 'value' is not of type 'number', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return DoubleTypeValue(std::move(value));
}

string DoubleTypeValue::TryFromJSON(yyjson_val *obj, optional<DoubleTypeValue> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DoubleTypeValue DoubleTypeValue::Copy() const {
	double value_tmp;
	value_tmp = value;
	return DoubleTypeValue(std::move(value_tmp));
}

string DoubleTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *DoubleTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_real(doc, value);
}

} // namespace rest_api_objects
} // namespace duckdb
