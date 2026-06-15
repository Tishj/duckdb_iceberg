
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/view_metadata.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class LoadViewResult {
public:
	LoadViewResult();
	LoadViewResult(const LoadViewResult &) = delete;
	LoadViewResult &operator=(const LoadViewResult &) = delete;
	LoadViewResult(LoadViewResult &&) = default;
	LoadViewResult &operator=(LoadViewResult &&) = default;

public:
	// Deserialization
	static LoadViewResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	LoadViewResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string metadata_location;
	ViewMetadata metadata;
	optional<case_insensitive_map_t<string>> config;
};

class LoadViewResultBuilder {
public:
	LoadViewResultBuilder();
	LoadViewResultBuilder &SetMetadataLocation(string value);
	LoadViewResultBuilder &SetMetadata(ViewMetadata value);
	LoadViewResultBuilder &SetConfig(case_insensitive_map_t<string> value);
	string TryBuild(LoadViewResult &result);
	LoadViewResult Build();

private:
	LoadViewResult result_;
	bool has_metadata_location_ = false;
	bool has_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
