
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RemoveEncryptionKeyUpdateBuilder;

class RemoveEncryptionKeyUpdate {
public:
	RemoveEncryptionKeyUpdate(const RemoveEncryptionKeyUpdate &) = delete;
	RemoveEncryptionKeyUpdate &operator=(const RemoveEncryptionKeyUpdate &) = delete;
	RemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate &&) = default;
	RemoveEncryptionKeyUpdate &operator=(RemoveEncryptionKeyUpdate &&) = delete;

private:
	friend class RemoveEncryptionKeyUpdateBuilder;
	RemoveEncryptionKeyUpdate(BaseUpdate base_update_p, string key_id_p);

public:
	// Deserialization
	static RemoveEncryptionKeyUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, RemoveEncryptionKeyUpdateBuilder &builder);
	string Validate() const;

	// Copy
	RemoveEncryptionKeyUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	string key_id;
};

class RemoveEncryptionKeyUpdateBuilder {
public:
	RemoveEncryptionKeyUpdateBuilder();
	RemoveEncryptionKeyUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveEncryptionKeyUpdateBuilder &SetKeyId(string value);
	string TryBuild(optional<RemoveEncryptionKeyUpdate> &result);
	RemoveEncryptionKeyUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<string> key_id_;
	bool has_key_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
