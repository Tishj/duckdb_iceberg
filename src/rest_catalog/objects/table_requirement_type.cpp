
#include "rest_catalog/objects/table_requirement_type.hpp"

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

TableRequirementType::TableRequirementType(string value_p) : value(std::move(value_p)) {
}
TableRequirementType::TableRequirementType(const TableRequirementType &other) : value(other.value) {
}
TableRequirementType::TableRequirementType(TableRequirementType &&other)
    : TableRequirementType(static_cast<const TableRequirementType &>(other)) {
}

optional<string> TableRequirementType::TryFromJSON(yyjson_val *obj, optional<TableRequirementType> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("TableRequirementType property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(TableRequirementType(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableRequirementType TableRequirementType::FromJSON(yyjson_val *obj) {
	optional<TableRequirementType> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

TableRequirementType TableRequirementType::Copy() const {
	return TableRequirementType(*this);
}

optional<string> TableRequirementType::Validate() const {
	optional<string> error;
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
	return nullopt;
}

yyjson_mut_val *TableRequirementType::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
