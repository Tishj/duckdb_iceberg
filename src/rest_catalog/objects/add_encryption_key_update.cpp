
#include "rest_catalog/objects/add_encryption_key_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AddEncryptionKeyUpdate::AddEncryptionKeyUpdate()
    : base_update(GeneratedObjectAccess::Create<BaseUpdate>()),
      encryption_key(GeneratedObjectAccess::Create<EncryptedKey>()) {
}

AddEncryptionKeyUpdateBuilder::AddEncryptionKeyUpdateBuilder() {
}

AddEncryptionKeyUpdateBuilder &AddEncryptionKeyUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

AddEncryptionKeyUpdateBuilder &AddEncryptionKeyUpdateBuilder::SetEncryptionKey(EncryptedKey value) {
	result_.encryption_key = std::move(value);
	has_encryption_key_ = true;
	return *this;
}

string AddEncryptionKeyUpdateBuilder::TryBuild(AddEncryptionKeyUpdate &result) {
	if (!has_encryption_key_) {
		return "AddEncryptionKeyUpdate required property 'encryption-key' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdateBuilder::Build() {
	AddEncryptionKeyUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdate::FromJSON(yyjson_val *obj) {
	AddEncryptionKeyUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdate::Copy() const {
	AddEncryptionKeyUpdate res;
	res.base_update = base_update.Copy();
	res.encryption_key = encryption_key.Copy();
	return res;
}

string AddEncryptionKeyUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = encryption_key.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string AddEncryptionKeyUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto encryption_key_val = yyjson_obj_get(obj, "encryption-key");
	if (!encryption_key_val) {
		return "AddEncryptionKeyUpdate required property 'encryption-key' is missing";
	} else {
		error = encryption_key.TryFromJSON(encryption_key_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void AddEncryptionKeyUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: encryption-key
	yyjson_mut_val *encryption_key_val = encryption_key.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "encryption-key", encryption_key_val);
}

yyjson_mut_val *AddEncryptionKeyUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
