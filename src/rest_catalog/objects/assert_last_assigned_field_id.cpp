
#include "rest_catalog/objects/assert_last_assigned_field_id.hpp"

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

AssertLastAssignedFieldId::AssertLastAssignedFieldId(TableRequirementType type_p, int32_t last_assigned_field_id_p)
    : type(std::move(type_p)), last_assigned_field_id(std::move(last_assigned_field_id_p)) {
}
AssertLastAssignedFieldId::AssertLastAssignedFieldId(const AssertLastAssignedFieldId &other)
    : type(other.type.Copy()), last_assigned_field_id(other.last_assigned_field_id) {
}
AssertLastAssignedFieldId::AssertLastAssignedFieldId(AssertLastAssignedFieldId &&other)
    : AssertLastAssignedFieldId(static_cast<const AssertLastAssignedFieldId &>(other)) {
}

AssertLastAssignedFieldIdBuilder::AssertLastAssignedFieldIdBuilder() {
}

AssertLastAssignedFieldIdBuilder &AssertLastAssignedFieldIdBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertLastAssignedFieldIdBuilder &AssertLastAssignedFieldIdBuilder::SetLastAssignedFieldId(int32_t value) {
	last_assigned_field_id_.emplace(std::move(value));
	has_last_assigned_field_id_ = true;
	return *this;
}

AssertLastAssignedFieldId AssertLastAssignedFieldIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertLastAssignedFieldId required property 'type' is missing");
	}
	if (!has_last_assigned_field_id_) {
		throw InvalidInputException("AssertLastAssignedFieldId required property 'last-assigned-field-id' is missing");
	}
	auto result = AssertLastAssignedFieldId(std::move(*type_), std::move(*last_assigned_field_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> AssertLastAssignedFieldIdBuilder::TryBuild(optional<AssertLastAssignedFieldId> &result) {
	if (!has_type_) {
		return "AssertLastAssignedFieldId required property 'type' is missing";
	}
	if (!has_last_assigned_field_id_) {
		return "AssertLastAssignedFieldId required property 'last-assigned-field-id' is missing";
	}
	auto built = AssertLastAssignedFieldId(std::move(*type_), std::move(*last_assigned_field_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> AssertLastAssignedFieldId::TryFromJSON(yyjson_val *obj, AssertLastAssignedFieldIdBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertLastAssignedFieldId required property 'type' is missing");
		} else {
			builder.SetType(TableRequirementType::FromJSON(type_val));
		}
		auto last_assigned_field_id_val = yyjson_obj_get(obj, "last-assigned-field-id");
		if (!last_assigned_field_id_val) {
			throw InvalidInputException(
			    "AssertLastAssignedFieldId required property 'last-assigned-field-id' is missing");
		} else {
			int32_t last_assigned_field_id;
			if (yyjson_is_int(last_assigned_field_id_val)) {
				last_assigned_field_id = yyjson_get_int(last_assigned_field_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertLastAssignedFieldId property 'last_assigned_field_id' is not of type "
				                       "'integer', found '%s' instead",
				                       yyjson_get_type_desc(last_assigned_field_id_val)));
			}
			builder.SetLastAssignedFieldId(std::move(last_assigned_field_id));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertLastAssignedFieldId AssertLastAssignedFieldId::FromJSON(yyjson_val *obj) {
	AssertLastAssignedFieldIdBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

AssertLastAssignedFieldId AssertLastAssignedFieldId::Copy() const {
	return AssertLastAssignedFieldId(*this);
}

optional<string> AssertLastAssignedFieldId::Validate() const {
	optional<string> error;
	error = type.Validate();
	if (error) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-last-assigned-field-id")) {
		return StringUtil::Format(
		    "AssertLastAssignedFieldId property 'type' must be assert-last-assigned-field-id, not %s", type.value);
	}
	return nullopt;
}

void AssertLastAssignedFieldId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: last-assigned-field-id
	yyjson_mut_obj_add_int(doc, obj, "last-assigned-field-id", last_assigned_field_id);
}

yyjson_mut_val *AssertLastAssignedFieldId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
