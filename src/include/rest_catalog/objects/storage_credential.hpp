
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

class StorageCredentialBuilder;

class StorageCredential {
public:
	StorageCredential(const StorageCredential &);
	StorageCredential &operator=(const StorageCredential &) = delete;
	StorageCredential(StorageCredential &&);
	StorageCredential &operator=(StorageCredential &&) = delete;

private:
	friend class StorageCredentialBuilder;
	StorageCredential(string prefix_p, case_insensitive_map_t<string> config_p);

public:
	// Deserialization
	static StorageCredential FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, StorageCredentialBuilder &builder);
	optional<string> Validate() const;

	// Copy
	StorageCredential Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string prefix;
	const case_insensitive_map_t<string> config;
};

class StorageCredentialBuilder {
public:
	StorageCredentialBuilder();
	StorageCredentialBuilder &SetPrefix(string value);
	StorageCredentialBuilder &SetConfig(case_insensitive_map_t<string> value);
	optional<string> TryBuild(optional<StorageCredential> &result);
	StorageCredential Build();

private:
	optional<string> prefix_;
	optional<case_insensitive_map_t<string>> config_;
	bool has_prefix_ = false;
	bool has_config_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
