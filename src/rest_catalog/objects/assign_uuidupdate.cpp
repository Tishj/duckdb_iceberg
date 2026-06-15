
#include "rest_catalog/objects/assign_uuidupdate.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssignUUIDUpdate::AssignUUIDUpdate() {
}

AssignUUIDUpdateBuilder::AssignUUIDUpdateBuilder() {
}

AssignUUIDUpdateBuilder &AssignUUIDUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

AssignUUIDUpdateBuilder &AssignUUIDUpdateBuilder::SetUuid(string value) {
	result_.uuid = std::move(value);
	has_uuid_ = true;
	return *this;
}

string AssignUUIDUpdateBuilder::TryBuild(AssignUUIDUpdate &result) {
	if (!has_uuid_) {
		return "AssignUUIDUpdate required property 'uuid' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssignUUIDUpdate AssignUUIDUpdateBuilder::Build() {
	AssignUUIDUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssignUUIDUpdate AssignUUIDUpdate::FromJSON(yyjson_val *obj) {
	AssignUUIDUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssignUUIDUpdate AssignUUIDUpdate::Copy() const {
	AssignUUIDUpdate res;
	res.base_update = base_update.Copy();
	res.uuid = uuid;
	return res;
}

string AssignUUIDUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string AssignUUIDUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto uuid_val = yyjson_obj_get(obj, "uuid");
	if (!uuid_val) {
		return "AssignUUIDUpdate required property 'uuid' is missing";
	} else {
		if (yyjson_is_str(uuid_val)) {
			uuid = yyjson_get_str(uuid_val);
		} else {
			return StringUtil::Format("AssignUUIDUpdate property 'uuid' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(uuid_val));
		}
	}
	return Validate();
}

void AssignUUIDUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: uuid
	yyjson_mut_obj_add_strcpy(doc, obj, "uuid", uuid.c_str());
}

yyjson_mut_val *AssignUUIDUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
