
#include "rest_catalog/objects/assert_table_uuid.hpp"

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

AssertTableUUID::AssertTableUUID(TableRequirementType type_p, string uuid_p)
    : type(std::move(type_p)), uuid(std::move(uuid_p)) {
}

AssertTableUUIDBuilder::AssertTableUUIDBuilder() {
}

AssertTableUUIDBuilder &AssertTableUUIDBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertTableUUIDBuilder &AssertTableUUIDBuilder::SetUuid(string value) {
	uuid_.emplace(std::move(value));
	has_uuid_ = true;
	return *this;
}

AssertTableUUID AssertTableUUIDBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertTableUUID required property 'type' is missing");
	}
	if (!has_uuid_) {
		throw InvalidInputException("AssertTableUUID required property 'uuid' is missing");
	}
	auto result = AssertTableUUID(std::move(*type_), std::move(*uuid_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertTableUUIDBuilder::TryBuild(optional<AssertTableUUID> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssertTableUUID::TryFromJSON(yyjson_val *obj, AssertTableUUIDBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertTableUUID required property 'type' is missing");
		} else {
			builder.SetType(TableRequirementType::FromJSON(type_val));
		}
		auto uuid_val = yyjson_obj_get(obj, "uuid");
		if (!uuid_val) {
			throw InvalidInputException("AssertTableUUID required property 'uuid' is missing");
		} else {
			string uuid;
			if (yyjson_is_str(uuid_val)) {
				uuid = yyjson_get_str(uuid_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertTableUUID property 'uuid' is not of type 'string', found '%s' instead",
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

AssertTableUUID AssertTableUUID::FromJSON(yyjson_val *obj) {
	AssertTableUUIDBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssertTableUUID AssertTableUUID::Copy() const {
	AssertTableUUIDBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	string uuid_tmp;
	uuid_tmp = uuid;
	builder.SetUuid(std::move(uuid_tmp));
	return builder.Build();
}

string AssertTableUUID::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-table-uuid")) {
		return StringUtil::Format("AssertTableUUID property 'type' must be assert-table-uuid, not %s", type.value);
	}
	return "";
}

void AssertTableUUID::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: uuid
	yyjson_mut_obj_add_strcpy(doc, obj, "uuid", uuid.c_str());
}

yyjson_mut_val *AssertTableUUID::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
