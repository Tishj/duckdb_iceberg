
#include "rest_catalog/objects/assert_current_schema_id.hpp"

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

AssertCurrentSchemaId::AssertCurrentSchemaId(TableRequirementType type_p, int32_t current_schema_id_p)
    : type(std::move(type_p)), current_schema_id(std::move(current_schema_id_p)) {
}

AssertCurrentSchemaIdBuilder::AssertCurrentSchemaIdBuilder() {
}

AssertCurrentSchemaIdBuilder &AssertCurrentSchemaIdBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertCurrentSchemaIdBuilder &AssertCurrentSchemaIdBuilder::SetCurrentSchemaId(int32_t value) {
	current_schema_id_.emplace(std::move(value));
	has_current_schema_id_ = true;
	return *this;
}

AssertCurrentSchemaId AssertCurrentSchemaIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertCurrentSchemaId required property 'type' is missing");
	}
	if (!has_current_schema_id_) {
		throw InvalidInputException("AssertCurrentSchemaId required property 'current-schema-id' is missing");
	}
	auto result = AssertCurrentSchemaId(std::move(*type_), std::move(*current_schema_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertCurrentSchemaIdBuilder::TryBuild(optional<AssertCurrentSchemaId> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssertCurrentSchemaId::TryFromJSON(yyjson_val *obj, AssertCurrentSchemaIdBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertCurrentSchemaId required property 'type' is missing");
		} else {
			builder.SetType(TableRequirementType::FromJSON(type_val));
		}
		auto current_schema_id_val = yyjson_obj_get(obj, "current-schema-id");
		if (!current_schema_id_val) {
			throw InvalidInputException("AssertCurrentSchemaId required property 'current-schema-id' is missing");
		} else {
			int32_t current_schema_id;
			if (yyjson_is_int(current_schema_id_val)) {
				current_schema_id = yyjson_get_int(current_schema_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "AssertCurrentSchemaId property 'current_schema_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(current_schema_id_val)));
			}
			builder.SetCurrentSchemaId(std::move(current_schema_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertCurrentSchemaId AssertCurrentSchemaId::FromJSON(yyjson_val *obj) {
	AssertCurrentSchemaIdBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssertCurrentSchemaId AssertCurrentSchemaId::Copy() const {
	AssertCurrentSchemaIdBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	int32_t current_schema_id_tmp;
	current_schema_id_tmp = current_schema_id;
	builder.SetCurrentSchemaId(std::move(current_schema_id_tmp));
	return builder.Build();
}

string AssertCurrentSchemaId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-current-schema-id")) {
		return StringUtil::Format("AssertCurrentSchemaId property 'type' must be assert-current-schema-id, not %s",
		                          type.value);
	}
	return "";
}

void AssertCurrentSchemaId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: current-schema-id
	yyjson_mut_obj_add_int(doc, obj, "current-schema-id", current_schema_id);
}

yyjson_mut_val *AssertCurrentSchemaId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
