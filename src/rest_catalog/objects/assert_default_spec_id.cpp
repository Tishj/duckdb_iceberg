
#include "rest_catalog/objects/assert_default_spec_id.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssertDefaultSpecId::AssertDefaultSpecId(TableRequirementType type_p, int32_t default_spec_id_p)
    : type(std::move(type_p)), default_spec_id(std::move(default_spec_id_p)) {
}

AssertDefaultSpecIdBuilder::AssertDefaultSpecIdBuilder() {
}

AssertDefaultSpecIdBuilder &AssertDefaultSpecIdBuilder::SetType(TableRequirementType value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

AssertDefaultSpecIdBuilder &AssertDefaultSpecIdBuilder::SetDefaultSpecId(int32_t value) {
	default_spec_id_ = std::move(value);
	has_default_spec_id_ = true;
	return *this;
}

AssertDefaultSpecId AssertDefaultSpecIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertDefaultSpecId required property 'type' is missing");
	}
	if (!has_default_spec_id_) {
		throw InvalidInputException("AssertDefaultSpecId required property 'default-spec-id' is missing");
	}
	auto result = AssertDefaultSpecId(std::move(*type_), std::move(*default_spec_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertDefaultSpecIdBuilder::TryBuild(optional<AssertDefaultSpecId> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertDefaultSpecId AssertDefaultSpecId::FromJSON(yyjson_val *obj) {
	AssertDefaultSpecIdBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("AssertDefaultSpecId required property 'type' is missing");
	} else {
		optional<TableRequirementType> type;
		type = TableRequirementType::FromJSON(type_val);
		builder.SetType(std::move(*type));
	}
	auto default_spec_id_val = yyjson_obj_get(obj, "default-spec-id");
	if (!default_spec_id_val) {
		throw InvalidInputException("AssertDefaultSpecId required property 'default-spec-id' is missing");
	} else {
		int32_t default_spec_id;
		if (yyjson_is_int(default_spec_id_val)) {
			default_spec_id = yyjson_get_int(default_spec_id_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "AssertDefaultSpecId property 'default_spec_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(default_spec_id_val)));
		}
		builder.SetDefaultSpecId(std::move(default_spec_id));
	}
	return builder.Build();
}

string AssertDefaultSpecId::TryFromJSON(yyjson_val *obj, optional<AssertDefaultSpecId> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertDefaultSpecId AssertDefaultSpecId::Copy() const {
	AssertDefaultSpecIdBuilder builder;
	optional<TableRequirementType> type_tmp;
	type_tmp = type.Copy();
	builder.SetType(std::move(*type_tmp));
	int32_t default_spec_id_tmp;
	default_spec_id_tmp = default_spec_id;
	builder.SetDefaultSpecId(std::move(default_spec_id_tmp));
	return builder.Build();
}

string AssertDefaultSpecId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-default-spec-id")) {
		return StringUtil::Format("AssertDefaultSpecId property 'type' must be assert-default-spec-id, not %s",
		                          type.value);
	}
	return "";
}

void AssertDefaultSpecId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: default-spec-id
	yyjson_mut_obj_add_int(doc, obj, "default-spec-id", default_spec_id);
}

yyjson_mut_val *AssertDefaultSpecId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
