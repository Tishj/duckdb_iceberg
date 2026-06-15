
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CatalogConfig {
public:
	CatalogConfig();
	CatalogConfig(const CatalogConfig &) = delete;
	CatalogConfig &operator=(const CatalogConfig &) = delete;
	CatalogConfig(CatalogConfig &&) = default;
	CatalogConfig &operator=(CatalogConfig &&) = default;

public:
	// Deserialization
	static CatalogConfig FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CatalogConfig Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	case_insensitive_map_t<string> defaults;
	case_insensitive_map_t<string> overrides;
	optional<vector<string>> endpoints;
	optional<string> idempotency_key_lifetime;
};

class CatalogConfigBuilder {
public:
	CatalogConfigBuilder();
	CatalogConfigBuilder &SetDefaults(case_insensitive_map_t<string> value);
	CatalogConfigBuilder &SetOverrides(case_insensitive_map_t<string> value);
	CatalogConfigBuilder &SetEndpoints(vector<string> value);
	CatalogConfigBuilder &SetIdempotencyKeyLifetime(string value);
	string TryBuild(CatalogConfig &result);
	CatalogConfig Build();

private:
	CatalogConfig result_;
	bool has_defaults_ = false;
	bool has_overrides_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
