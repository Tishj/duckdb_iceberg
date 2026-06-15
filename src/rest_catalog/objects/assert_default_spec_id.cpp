
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

AssertDefaultSpecId::AssertDefaultSpecId() {
}

AssertDefaultSpecIdBuilder::AssertDefaultSpecIdBuilder() {
}

AssertDefaultSpecIdBuilder &AssertDefaultSpecIdBuilder::SetType(TableRequirementType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AssertDefaultSpecIdBuilder &AssertDefaultSpecIdBuilder::SetDefaultSpecId(int32_t value) {
	result_.default_spec_id = std::move(value);
	has_default_spec_id_ = true;
	return *this;
}

string AssertDefaultSpecIdBuilder::TryBuild(AssertDefaultSpecId &result) {
	if (!has_type_) {
		return "AssertDefaultSpecId required property 'type' is missing";
	}
	if (!has_default_spec_id_) {
		return "AssertDefaultSpecId required property 'default-spec-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssertDefaultSpecId AssertDefaultSpecIdBuilder::Build() {
	AssertDefaultSpecId result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssertDefaultSpecId AssertDefaultSpecId::FromJSON(yyjson_val *obj) {
	AssertDefaultSpecId res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssertDefaultSpecId AssertDefaultSpecId::Copy() const {
	AssertDefaultSpecId res;
	res.type = type.Copy();
	res.default_spec_id = default_spec_id;
	return res;
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

string AssertDefaultSpecId::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AssertDefaultSpecId required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto default_spec_id_val = yyjson_obj_get(obj, "default-spec-id");
	if (!default_spec_id_val) {
		return "AssertDefaultSpecId required property 'default-spec-id' is missing";
	} else {
		if (yyjson_is_int(default_spec_id_val)) {
			default_spec_id = yyjson_get_int(default_spec_id_val);
		} else {
			return StringUtil::Format(
			    "AssertDefaultSpecId property 'default_spec_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(default_spec_id_val));
		}
	}
	return Validate();
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
