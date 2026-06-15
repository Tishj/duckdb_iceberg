
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/encrypted_key.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddEncryptionKeyUpdate {
public:
	AddEncryptionKeyUpdate();
	AddEncryptionKeyUpdate(const AddEncryptionKeyUpdate &) = delete;
	AddEncryptionKeyUpdate &operator=(const AddEncryptionKeyUpdate &) = delete;
	AddEncryptionKeyUpdate(AddEncryptionKeyUpdate &&) = default;
	AddEncryptionKeyUpdate &operator=(AddEncryptionKeyUpdate &&) = default;

public:
	// Deserialization
	static AddEncryptionKeyUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	AddEncryptionKeyUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	EncryptedKey encryption_key;
};

class AddEncryptionKeyUpdateBuilder {
public:
	AddEncryptionKeyUpdateBuilder();
	AddEncryptionKeyUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddEncryptionKeyUpdateBuilder &SetEncryptionKey(EncryptedKey value);
	string TryBuild(AddEncryptionKeyUpdate &result);
	AddEncryptionKeyUpdate Build();

private:
	AddEncryptionKeyUpdate result_;
	bool has_encryption_key_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
