
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
	LoadTableResult(const LoadTableResult &);
	LoadTableResult &operator=(const LoadTableResult &) = delete;
	LoadTableResult(LoadTableResult &&);
	LoadTableResult &operator=(LoadTableResult &&) = delete;

private:
	friend class LoadTableResultBuilder;
	LoadTableResult(TableMetadata metadata_p, optional<string> metadata_location_p,
	                optional<case_insensitive_map_t<string>> config_p,
	                optional<vector<StorageCredential>> storage_credentials_p);

public:
	// Deserialization
	static LoadTableResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, LoadTableResultBuilder &builder);
	string Validate() const;

	// Copy
	LoadTableResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableMetadata metadata;
	const optional<string> metadata_location;
	const optional<case_insensitive_map_t<string>> config;
	const optional<vector<StorageCredential>> storage_credentials;
};

class LoadTableResultBuilder {
public:
	LoadTableResultBuilder();
	LoadTableResultBuilder &SetMetadata(TableMetadata value);
	LoadTableResultBuilder &SetMetadataLocation(string value);
	LoadTableResultBuilder &SetConfig(case_insensitive_map_t<string> value);
	LoadTableResultBuilder &SetStorageCredentials(vector<StorageCredential> value);
	string TryBuild(optional<LoadTableResult> &result);
	LoadTableResult Build();

private:
	optional<TableMetadata> metadata_;
	optional<string> metadata_location_;
	optional<case_insensitive_map_t<string>> config_;
	optional<vector<StorageCredential>> storage_credentials_;
	bool has_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
