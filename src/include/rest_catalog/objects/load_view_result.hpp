
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/view_metadata.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class LoadViewResultBuilder;

class LoadViewResult {
public:
	LoadViewResult(const LoadViewResult &);
	LoadViewResult &operator=(const LoadViewResult &) = delete;
	LoadViewResult(LoadViewResult &&);
	LoadViewResult &operator=(LoadViewResult &&) = delete;

private:
	friend class LoadViewResultBuilder;
	LoadViewResult(string metadata_location_p, ViewMetadata metadata_p,
	               optional<case_insensitive_map_t<string>> config_p);

public:
	// Deserialization
	static LoadViewResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, LoadViewResultBuilder &builder);
	string Validate() const;

	// Copy
	LoadViewResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string metadata_location;
	const ViewMetadata metadata;
	const optional<case_insensitive_map_t<string>> config;
};

class LoadViewResultBuilder {
public:
	LoadViewResultBuilder();
	LoadViewResultBuilder &SetMetadataLocation(string value);
	LoadViewResultBuilder &SetMetadata(ViewMetadata value);
	LoadViewResultBuilder &SetConfig(case_insensitive_map_t<string> value);
	string TryBuild(optional<LoadViewResult> &result);
	LoadViewResult Build();

private:
	optional<string> metadata_location_;
	optional<ViewMetadata> metadata_;
	optional<case_insensitive_map_t<string>> config_;
	bool has_metadata_location_ = false;
	bool has_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
