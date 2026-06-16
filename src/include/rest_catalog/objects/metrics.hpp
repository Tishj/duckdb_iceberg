
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/metric_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class MetricsBuilder;

class Metrics {
public:
	Metrics(const Metrics &);
	Metrics &operator=(const Metrics &) = delete;
	Metrics(Metrics &&);
	Metrics &operator=(Metrics &&) = delete;

private:
	friend class MetricsBuilder;
	Metrics(case_insensitive_map_t<MetricResult> additional_properties_p);

public:
	// Deserialization
	static Metrics FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, MetricsBuilder &builder);
	string Validate() const;

	// Copy
	Metrics Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const case_insensitive_map_t<MetricResult> additional_properties;
};

class MetricsBuilder {
public:
	MetricsBuilder();
	MetricsBuilder &SetAdditionalProperties(case_insensitive_map_t<MetricResult> value);
	string TryBuild(optional<Metrics> &result);
	Metrics Build();

private:
	optional<case_insensitive_map_t<MetricResult>> additional_properties_;
};

} // namespace rest_api_objects
} // namespace duckdb
