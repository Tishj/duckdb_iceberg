
#include "rest_catalog/objects/table_requirement_type.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TableRequirementType::TableRequirementType() {
}

TableRequirementType TableRequirementType::FromJSON(yyjson_val *obj) {
	TableRequirementType res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

TableRequirementType TableRequirementType::Copy() const {
	TableRequirementType res;
	res.value = value;
	return res;
}

string TableRequirementType::Validate() const {
	string error;
	if (!StringUtil::CIEquals(value, "assert-create") && !StringUtil::CIEquals(value, "assert-table-uuid") &&
	    !StringUtil::CIEquals(value, "assert-ref-snapshot-id") &&
	    !StringUtil::CIEquals(value, "assert-last-assigned-field-id") &&
	    !StringUtil::CIEquals(value, "assert-current-schema-id") &&
	    !StringUtil::CIEquals(value, "assert-last-assigned-partition-id") &&
	    !StringUtil::CIEquals(value, "assert-default-spec-id") &&
	    !StringUtil::CIEquals(value, "assert-default-sort-order-id")) {
		return StringUtil::Format(
		    "TableRequirementType property 'value' must be one of [assert-create, assert-table-uuid, "
		    "assert-ref-snapshot-id, assert-last-assigned-field-id, assert-current-schema-id, "
		    "assert-last-assigned-partition-id, assert-default-spec-id, assert-default-sort-order-id], not %s",
		    value);
	}
	return "";
}

string TableRequirementType::TryFromJSON(yyjson_val *obj) {
	string error;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		return StringUtil::Format("TableRequirementType property 'value' is not of type 'string', found '%s' instead",
		                          yyjson_get_type_desc(obj));
	}
	return Validate();
}

yyjson_mut_val *TableRequirementType::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
