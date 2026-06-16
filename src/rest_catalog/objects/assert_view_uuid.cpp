
#include "rest_catalog/objects/assert_view_uuid.hpp"

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

AssertViewUUID::AssertViewUUID(string type_p, string uuid_p) : type(std::move(type_p)), uuid(std::move(uuid_p)) {
}
AssertViewUUID::AssertViewUUID(const AssertViewUUID &other) : type(other.type), uuid(other.uuid) {
}
AssertViewUUID::AssertViewUUID(AssertViewUUID &&other) : AssertViewUUID(static_cast<const AssertViewUUID &>(other)) {
}

AssertViewUUIDBuilder::AssertViewUUIDBuilder() {
}

AssertViewUUIDBuilder &AssertViewUUIDBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertViewUUIDBuilder &AssertViewUUIDBuilder::SetUuid(string value) {
	uuid_.emplace(std::move(value));
	has_uuid_ = true;
	return *this;
}

AssertViewUUID AssertViewUUIDBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertViewUUID required property 'type' is missing");
	}
	if (!has_uuid_) {
		throw InvalidInputException("AssertViewUUID required property 'uuid' is missing");
	}
	auto result = AssertViewUUID(std::move(*type_), std::move(*uuid_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertViewUUIDBuilder::TryBuild(optional<AssertViewUUID> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssertViewUUID::TryFromJSON(yyjson_val *obj, AssertViewUUIDBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertViewUUID required property 'type' is missing");
		} else {
			string type;
			if (yyjson_is_str(type_val)) {
				type = yyjson_get_str(type_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertViewUUID property 'type' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(type_val)));
			}
			builder.SetType(std::move(type));
		}
		auto uuid_val = yyjson_obj_get(obj, "uuid");
		if (!uuid_val) {
			throw InvalidInputException("AssertViewUUID required property 'uuid' is missing");
		} else {
			string uuid;
			if (yyjson_is_str(uuid_val)) {
				uuid = yyjson_get_str(uuid_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertViewUUID property 'uuid' is not of type 'string', found '%s' instead",
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

AssertViewUUID AssertViewUUID::FromJSON(yyjson_val *obj) {
	AssertViewUUIDBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssertViewUUID AssertViewUUID::Copy() const {
	return AssertViewUUID(*this);
}

string AssertViewUUID::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "assert-view-uuid")) {
		return StringUtil::Format("AssertViewUUID property 'type' must be assert-view-uuid, not %s", type);
	}
	return "";
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
