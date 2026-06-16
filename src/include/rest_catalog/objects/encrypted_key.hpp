
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class EncryptedKeyBuilder;

class EncryptedKey {
public:
	EncryptedKey(const EncryptedKey &) = delete;
	EncryptedKey &operator=(const EncryptedKey &) = delete;
	EncryptedKey(EncryptedKey &&) = default;
	EncryptedKey &operator=(EncryptedKey &&) = delete;

private:
	friend class EncryptedKeyBuilder;
	EncryptedKey(string key_id_p, string encrypted_key_metadata_p, optional<string> encrypted_by_id_p,
	             optional<case_insensitive_map_t<string>> properties_p);

public:
	// Deserialization
	static EncryptedKey FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, EncryptedKeyBuilder &builder);
	string Validate() const;

	// Copy
	EncryptedKey Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string key_id;
	string encrypted_key_metadata;
	optional<string> encrypted_by_id;
	optional<case_insensitive_map_t<string>> properties;
};

class EncryptedKeyBuilder {
public:
	EncryptedKeyBuilder();
	EncryptedKeyBuilder &SetKeyId(string value);
	EncryptedKeyBuilder &SetEncryptedKeyMetadata(string value);
	EncryptedKeyBuilder &SetEncryptedById(string value);
	EncryptedKeyBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(optional<EncryptedKey> &result);
	EncryptedKey Build();

private:
	optional<string> key_id_;
	optional<string> encrypted_key_metadata_;
	optional<string> encrypted_by_id_;
	optional<case_insensitive_map_t<string>> properties_;
	bool has_key_id_ = false;
	bool has_encrypted_key_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
