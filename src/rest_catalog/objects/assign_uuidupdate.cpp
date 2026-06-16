
#include "rest_catalog/objects/assign_uuidupdate.hpp"

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

AssignUUIDUpdate::AssignUUIDUpdate(BaseUpdate base_update_p, string uuid_p)
    : base_update(std::move(base_update_p)), uuid(std::move(uuid_p)) {
}

AssignUUIDUpdateBuilder::AssignUUIDUpdateBuilder() {
}

AssignUUIDUpdateBuilder &AssignUUIDUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AssignUUIDUpdateBuilder &AssignUUIDUpdateBuilder::SetUuid(string value) {
	uuid_.emplace(std::move(value));
	has_uuid_ = true;
	return *this;
}

AssignUUIDUpdate AssignUUIDUpdateBuilder::Build() {
	if (!has_uuid_) {
		throw InvalidInputException("AssignUUIDUpdate required property 'uuid' is missing");
	}
	auto result = AssignUUIDUpdate(std::move(*base_update_), std::move(*uuid_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssignUUIDUpdateBuilder::TryBuild(optional<AssignUUIDUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssignUUIDUpdate::TryFromJSON(yyjson_val *obj, AssignUUIDUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto uuid_val = yyjson_obj_get(obj, "uuid");
		if (!uuid_val) {
			throw InvalidInputException("AssignUUIDUpdate required property 'uuid' is missing");
		} else {
			string uuid;
			if (yyjson_is_str(uuid_val)) {
				uuid = yyjson_get_str(uuid_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssignUUIDUpdate property 'uuid' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(uuid_val)));
			}
			builder.SetUuid(std::move(uuid));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssignUUIDUpdate AssignUUIDUpdate::FromJSON(yyjson_val *obj) {
	AssignUUIDUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssignUUIDUpdate AssignUUIDUpdate::Copy() const {
	AssignUUIDUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	string uuid_tmp;
	uuid_tmp = uuid;
	builder.SetUuid(std::move(uuid_tmp));
	return builder.Build();
}

string AssignUUIDUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
