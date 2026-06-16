
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

class CatalogConfigBuilder;

class CatalogConfig {
public:
	CatalogConfig(const CatalogConfig &);
	CatalogConfig &operator=(const CatalogConfig &) = delete;
	CatalogConfig(CatalogConfig &&);
	CatalogConfig &operator=(CatalogConfig &&) = delete;

private:
	friend class CatalogConfigBuilder;
	CatalogConfig(case_insensitive_map_t<string> defaults_p, case_insensitive_map_t<string> overrides_p,
	              optional<vector<string>> endpoints_p, optional<string> idempotency_key_lifetime_p);

public:
	// Deserialization
	static CatalogConfig FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CatalogConfigBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CatalogConfig Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const case_insensitive_map_t<string> defaults;
	const case_insensitive_map_t<string> overrides;
	const optional<vector<string>> endpoints;
	const optional<string> idempotency_key_lifetime;
};

class CatalogConfigBuilder {
public:
	CatalogConfigBuilder();
	CatalogConfigBuilder &SetDefaults(case_insensitive_map_t<string> value);
	CatalogConfigBuilder &SetOverrides(case_insensitive_map_t<string> value);
	CatalogConfigBuilder &SetEndpoints(vector<string> value);
	CatalogConfigBuilder &SetIdempotencyKeyLifetime(string value);
	optional<string> TryBuild(optional<CatalogConfig> &result);
	CatalogConfig Build();

private:
	optional<case_insensitive_map_t<string>> defaults_;
	optional<case_insensitive_map_t<string>> overrides_;
	optional<vector<string>> endpoints_;
	optional<string> idempotency_key_lifetime_;
	bool has_defaults_ = false;
	bool has_overrides_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
