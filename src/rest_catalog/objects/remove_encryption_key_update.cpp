
#include "rest_catalog/objects/remove_encryption_key_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RemoveEncryptionKeyUpdate::RemoveEncryptionKeyUpdate() : base_update(GeneratedObjectAccess::Create<BaseUpdate>()) {
}

RemoveEncryptionKeyUpdateBuilder::RemoveEncryptionKeyUpdateBuilder() {
}

RemoveEncryptionKeyUpdateBuilder &RemoveEncryptionKeyUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

RemoveEncryptionKeyUpdateBuilder &RemoveEncryptionKeyUpdateBuilder::SetKeyId(string value) {
	result_.key_id = std::move(value);
	has_key_id_ = true;
	return *this;
}

string RemoveEncryptionKeyUpdateBuilder::TryBuild(RemoveEncryptionKeyUpdate &result) {
	if (!has_key_id_) {
		return "RemoveEncryptionKeyUpdate required property 'key-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdateBuilder::Build() {
	RemoveEncryptionKeyUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::FromJSON(yyjson_val *obj) {
	RemoveEncryptionKeyUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::Copy() const {
	RemoveEncryptionKeyUpdate res;
	res.base_update = base_update.Copy();
	res.key_id = key_id;
	return res;
}

string RemoveEncryptionKeyUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string RemoveEncryptionKeyUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto key_id_val = yyjson_obj_get(obj, "key-id");
	if (!key_id_val) {
		return "RemoveEncryptionKeyUpdate required property 'key-id' is missing";
	} else {
		if (yyjson_is_str(key_id_val)) {
			key_id = yyjson_get_str(key_id_val);
		} else {
			return StringUtil::Format(
			    "RemoveEncryptionKeyUpdate property 'key_id' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(key_id_val));
		}
	}
	return Validate();
}

void RemoveEncryptionKeyUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: key-id
	yyjson_mut_obj_add_strcpy(doc, obj, "key-id", key_id.c_str());
}

yyjson_mut_val *RemoveEncryptionKeyUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
