
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class StorageCredential {
public:
	StorageCredential();
	StorageCredential(const StorageCredential &) = delete;
	StorageCredential &operator=(const StorageCredential &) = delete;
	StorageCredential(StorageCredential &&) = default;
	StorageCredential &operator=(StorageCredential &&) = default;

public:
	// Deserialization
	static StorageCredential FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	StorageCredential Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string prefix;
	case_insensitive_map_t<string> config;
};

class StorageCredentialBuilder {
public:
	StorageCredentialBuilder();
	StorageCredentialBuilder &SetPrefix(string value);
	StorageCredentialBuilder &SetConfig(case_insensitive_map_t<string> value);
	string TryBuild(StorageCredential &result);
	StorageCredential Build();

private:
	StorageCredential result_;
	bool has_prefix_ = false;
	bool has_config_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
