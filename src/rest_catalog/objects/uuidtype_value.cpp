
#include "rest_catalog/objects/uuidtype_value.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

UUIDTypeValue::UUIDTypeValue() {
}

UUIDTypeValue UUIDTypeValue::FromJSON(yyjson_val *obj) {
	UUIDTypeValue res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

UUIDTypeValue UUIDTypeValue::Copy() const {
	UUIDTypeValue res;
	res.value = value;
	return res;
}

string UUIDTypeValue::Validate() const {
	string error;
	if (value.size() < 36) {
		return "UUIDTypeValue property 'value' must have at least 36 characters";
	}
	if (value.size() > 36) {
		return "UUIDTypeValue property 'value' must have at most 36 characters";
	}
	static const std::regex value_pattern("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$");
	if (!std::regex_match(value, value_pattern)) {
		return "UUIDTypeValue property 'value' does not match the required pattern";
	}
	return "";
}

string UUIDTypeValue::TryFromJSON(yyjson_val *obj) {
	string error;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		return StringUtil::Format("UUIDTypeValue property 'value' is not of type 'string', found '%s' instead",
		                          yyjson_get_type_desc(obj));
	}
	return Validate();
}

yyjson_mut_val *UUIDTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
