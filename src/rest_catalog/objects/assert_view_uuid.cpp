
#include "rest_catalog/objects/assert_view_uuid.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssertViewUUID::AssertViewUUID() {
}

AssertViewUUIDBuilder::AssertViewUUIDBuilder() {
}

AssertViewUUIDBuilder &AssertViewUUIDBuilder::SetType(string value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AssertViewUUIDBuilder &AssertViewUUIDBuilder::SetUuid(string value) {
	result_.uuid = std::move(value);
	has_uuid_ = true;
	return *this;
}

string AssertViewUUIDBuilder::TryBuild(AssertViewUUID &result) {
	if (!has_type_) {
		return "AssertViewUUID required property 'type' is missing";
	}
	if (!has_uuid_) {
		return "AssertViewUUID required property 'uuid' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssertViewUUID AssertViewUUIDBuilder::Build() {
	AssertViewUUID result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssertViewUUID AssertViewUUID::FromJSON(yyjson_val *obj) {
	AssertViewUUID res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssertViewUUID AssertViewUUID::Copy() const {
	AssertViewUUID res;
	res.type = type;
	res.uuid = uuid;
	return res;
}

string AssertViewUUID::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "assert-view-uuid")) {
		return StringUtil::Format("AssertViewUUID property 'type' must be assert-view-uuid, not %s", type);
	}
	return "";
}

string AssertViewUUID::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AssertViewUUID required property 'type' is missing";
	} else {
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			return StringUtil::Format("AssertViewUUID property 'type' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(type_val));
		}
	}
	auto uuid_val = yyjson_obj_get(obj, "uuid");
	if (!uuid_val) {
		return "AssertViewUUID required property 'uuid' is missing";
	} else {
		if (yyjson_is_str(uuid_val)) {
			uuid = yyjson_get_str(uuid_val);
		} else {
			return StringUtil::Format("AssertViewUUID property 'uuid' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(uuid_val));
		}
	}
	return Validate();
}

void AssertViewUUID::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: uuid
	yyjson_mut_obj_add_strcpy(doc, obj, "uuid", uuid.c_str());
}

yyjson_mut_val *AssertViewUUID::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
