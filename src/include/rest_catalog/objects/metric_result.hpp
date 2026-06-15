
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/counter_result.hpp"
#include "rest_catalog/objects/timer_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class MetricResultBuilder;

class MetricResult {
public:
	MetricResult(const MetricResult &) = delete;
	MetricResult &operator=(const MetricResult &) = delete;
	MetricResult(MetricResult &&) = default;
	MetricResult &operator=(MetricResult &&) = delete;

private:
	friend class MetricResultBuilder;
	MetricResult(optional<CounterResult> counter_result_p, optional<TimerResult> timer_result_p);

public:
	// Deserialization
	static MetricResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<MetricResult> &result);
	string Validate() const;

	// Copy
	MetricResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<CounterResult> counter_result;
	optional<TimerResult> timer_result;
};

class MetricResultBuilder {
public:
	MetricResultBuilder();
	MetricResultBuilder &SetCounterResult(CounterResult value);
	MetricResultBuilder &SetTimerResult(TimerResult value);
	string TryBuild(optional<MetricResult> &result);
	MetricResult Build();

private:
	optional<CounterResult> counter_result_;
	optional<TimerResult> timer_result_;
};

} // namespace rest_api_objects
} // namespace duckdb
