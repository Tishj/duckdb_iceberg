
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/storage_credential.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class LoadCredentialsResponseBuilder;

class LoadCredentialsResponse {
public:
	LoadCredentialsResponse(const LoadCredentialsResponse &) = delete;
	LoadCredentialsResponse &operator=(const LoadCredentialsResponse &) = delete;
	LoadCredentialsResponse(LoadCredentialsResponse &&) = default;
	LoadCredentialsResponse &operator=(LoadCredentialsResponse &&) = default;

private:
	friend class LoadCredentialsResponseBuilder;
	friend class GeneratedObjectAccess;
	LoadCredentialsResponse();

public:
	// Deserialization
	static LoadCredentialsResponse FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	LoadCredentialsResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<StorageCredential> storage_credentials;
};

class LoadCredentialsResponseBuilder {
public:
	LoadCredentialsResponseBuilder();
	LoadCredentialsResponseBuilder &SetStorageCredentials(vector<StorageCredential> value);
	string TryBuild(LoadCredentialsResponse &result);
	LoadCredentialsResponse Build();

private:
	LoadCredentialsResponse result_;
	bool has_storage_credentials_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
