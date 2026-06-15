
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/storage_credential.hpp"
#include "rest_catalog/objects/table_metadata.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class LoadTableResultBuilder;

class LoadTableResult {
public:
	LoadTableResult(const LoadTableResult &) = delete;
	LoadTableResult &operator=(const LoadTableResult &) = delete;
	LoadTableResult(LoadTableResult &&) = default;
	LoadTableResult &operator=(LoadTableResult &&) = default;

private:
	friend class LoadTableResultBuilder;
	friend class GeneratedObjectAccess;
	LoadTableResult();

public:
	// Deserialization
	static LoadTableResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	LoadTableResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	TableMetadata metadata;
	optional<string> metadata_location;
	optional<case_insensitive_map_t<string>> config;
	optional<vector<StorageCredential>> storage_credentials;
};

class LoadTableResultBuilder {
public:
	LoadTableResultBuilder();
	LoadTableResultBuilder &SetMetadata(TableMetadata value);
	LoadTableResultBuilder &SetMetadataLocation(string value);
	LoadTableResultBuilder &SetConfig(case_insensitive_map_t<string> value);
	LoadTableResultBuilder &SetStorageCredentials(vector<StorageCredential> value);
	string TryBuild(LoadTableResult &result);
	LoadTableResult Build();

private:
	LoadTableResult result_;
	bool has_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
