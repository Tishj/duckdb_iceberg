
#include "rest_catalog/objects/encrypted_key.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

EncryptedKey::EncryptedKey(string key_id_p, string encrypted_key_metadata_p, optional<string> encrypted_by_id_p,
                           optional<case_insensitive_map_t<string>> properties_p)
    : key_id(std::move(key_id_p)), encrypted_key_metadata(std::move(encrypted_key_metadata_p)),
      encrypted_by_id(std::move(encrypted_by_id_p)), properties(std::move(properties_p)) {
}

EncryptedKeyBuilder::EncryptedKeyBuilder() {
}

EncryptedKeyBuilder &EncryptedKeyBuilder::SetKeyId(string value) {
	key_id_ = std::move(value);
	has_key_id_ = true;
	return *this;
}

EncryptedKeyBuilder &EncryptedKeyBuilder::SetEncryptedKeyMetadata(string value) {
	encrypted_key_metadata_ = std::move(value);
	has_encrypted_key_metadata_ = true;
	return *this;
}

EncryptedKeyBuilder &EncryptedKeyBuilder::SetEncryptedById(string value) {
	encrypted_by_id_ = std::move(value);
	return *this;
}

EncryptedKeyBuilder &EncryptedKeyBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_ = std::move(value);
	return *this;
}

EncryptedKey EncryptedKeyBuilder::Build() {
	if (!has_key_id_) {
		throw InvalidInputException("EncryptedKey required property 'key-id' is missing");
	}
	if (!has_encrypted_key_metadata_) {
		throw InvalidInputException("EncryptedKey required property 'encrypted-key-metadata' is missing");
	}
	auto result = EncryptedKey(std::move(*key_id_), std::move(*encrypted_key_metadata_), std::move(encrypted_by_id_),
	                           std::move(properties_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string EncryptedKeyBuilder::TryBuild(optional<EncryptedKey> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

EncryptedKey EncryptedKey::FromJSON(yyjson_val *obj) {
	EncryptedKeyBuilder builder;
	auto key_id_val = yyjson_obj_get(obj, "key-id");
	if (!key_id_val) {
		throw InvalidInputException("EncryptedKey required property 'key-id' is missing");
	} else {
		string key_id;
		if (yyjson_is_str(key_id_val)) {
			key_id = yyjson_get_str(key_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("EncryptedKey property 'key_id' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(key_id_val)));
		}
		builder.SetKeyId(std::move(key_id));
	}
	auto encrypted_key_metadata_val = yyjson_obj_get(obj, "encrypted-key-metadata");
	if (!encrypted_key_metadata_val) {
		throw InvalidInputException("EncryptedKey required property 'encrypted-key-metadata' is missing");
	} else {
		string encrypted_key_metadata;
		if (yyjson_is_str(encrypted_key_metadata_val)) {
			encrypted_key_metadata = yyjson_get_str(encrypted_key_metadata_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "EncryptedKey property 'encrypted_key_metadata' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(encrypted_key_metadata_val)));
		}
		builder.SetEncryptedKeyMetadata(std::move(encrypted_key_metadata));
	}
	auto encrypted_by_id_val = yyjson_obj_get(obj, "encrypted-by-id");
	if (encrypted_by_id_val) {
		string encrypted_by_id;
		if (yyjson_is_str(encrypted_by_id_val)) {
			encrypted_by_id = yyjson_get_str(encrypted_by_id_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "EncryptedKey property 'encrypted_by_id' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(encrypted_by_id_val)));
		}
		builder.SetEncryptedById(std::move(encrypted_by_id));
	}
	auto properties_val = yyjson_obj_get(obj, "properties");
	if (properties_val) {
		case_insensitive_map_t<string> properties;
		if (yyjson_is_obj(properties_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(properties_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("EncryptedKey property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				properties.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("EncryptedKey property 'properties' is not of type 'object'");
		}
		builder.SetProperties(std::move(properties));
	}
	return builder.Build();
}

string EncryptedKey::TryFromJSON(yyjson_val *obj, optional<EncryptedKey> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

EncryptedKey EncryptedKey::Copy() const {
	EncryptedKeyBuilder builder;
	string key_id_tmp;
	key_id_tmp = key_id;
	builder.SetKeyId(std::move(key_id_tmp));
	string encrypted_key_metadata_tmp;
	encrypted_key_metadata_tmp = encrypted_key_metadata;
	builder.SetEncryptedKeyMetadata(std::move(encrypted_key_metadata_tmp));
	string encrypted_by_id_tmp;
	if (encrypted_by_id.has_value()) {
		encrypted_by_id_tmp.emplace();
		(*encrypted_by_id_tmp) = (*encrypted_by_id);
	}
	if (encrypted_by_id_tmp.has_value()) {
		builder.SetEncryptedById(std::move(encrypted_by_id_tmp));
	}
	case_insensitive_map_t<string> properties_tmp;
	if (properties.has_value()) {
		properties_tmp.emplace();
		for (auto &entry : (*properties)) {
			(*properties_tmp).emplace(entry.first, entry.second);
		}
	}
	if (properties_tmp.has_value()) {
		builder.SetProperties(std::move(properties_tmp));
	}
	return builder.Build();
}

string EncryptedKey::Validate() const {
	string error;
	return "";
}

void EncryptedKey::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: key-id
	yyjson_mut_obj_add_strcpy(doc, obj, "key-id", key_id.c_str());

	// Serialize: encrypted-key-metadata
	yyjson_mut_obj_add_strcpy(doc, obj, "encrypted-key-metadata", encrypted_key_metadata.c_str());

	// Serialize: encrypted-by-id
	if (encrypted_by_id.has_value()) {
		auto &encrypted_by_id_value = *encrypted_by_id;
		yyjson_mut_obj_add_strcpy(doc, obj, "encrypted-by-id", encrypted_by_id_value.c_str());
	}

	// Serialize: properties
	if (properties.has_value()) {
		auto &properties_value = *properties;
		yyjson_mut_val *properties_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : properties_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, properties_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "properties", properties_value_obj);
	}
}

yyjson_mut_val *EncryptedKey::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
