
#include "rest_catalog/objects/add_encryption_key_update.hpp"

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

AddEncryptionKeyUpdate::AddEncryptionKeyUpdate(BaseUpdate base_update_p, EncryptedKey encryption_key_p)
    : base_update(std::move(base_update_p)), encryption_key(std::move(encryption_key_p)) {
}
AddEncryptionKeyUpdate::AddEncryptionKeyUpdate(const AddEncryptionKeyUpdate &other)
    : base_update(other.base_update.Copy()), encryption_key(other.encryption_key.Copy()) {
}
AddEncryptionKeyUpdate::AddEncryptionKeyUpdate(AddEncryptionKeyUpdate &&other)
    : AddEncryptionKeyUpdate(static_cast<const AddEncryptionKeyUpdate &>(other)) {
}

AddEncryptionKeyUpdateBuilder::AddEncryptionKeyUpdateBuilder() {
}

AddEncryptionKeyUpdateBuilder &AddEncryptionKeyUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AddEncryptionKeyUpdateBuilder &AddEncryptionKeyUpdateBuilder::SetEncryptionKey(EncryptedKey value) {
	encryption_key_.emplace(std::move(value));
	has_encryption_key_ = true;
	return *this;
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdateBuilder::Build() {
	if (!has_encryption_key_) {
		throw InvalidInputException("AddEncryptionKeyUpdate required property 'encryption-key' is missing");
	}
	auto result = AddEncryptionKeyUpdate(std::move(*base_update_), std::move(*encryption_key_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AddEncryptionKeyUpdateBuilder::TryBuild(optional<AddEncryptionKeyUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AddEncryptionKeyUpdate::TryFromJSON(yyjson_val *obj, AddEncryptionKeyUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto encryption_key_val = yyjson_obj_get(obj, "encryption-key");
		if (!encryption_key_val) {
			throw InvalidInputException("AddEncryptionKeyUpdate required property 'encryption-key' is missing");
		} else {
			builder.SetEncryptionKey(EncryptedKey::FromJSON(encryption_key_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdate::FromJSON(yyjson_val *obj) {
	AddEncryptionKeyUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AddEncryptionKeyUpdate AddEncryptionKeyUpdate::Copy() const {
	return AddEncryptionKeyUpdate(*this);
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
