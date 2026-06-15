
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

class TimerResultBuilder;

class TimerResult {
public:
	TimerResult(const TimerResult &) = delete;
	TimerResult &operator=(const TimerResult &) = delete;
	TimerResult(TimerResult &&) = default;
	TimerResult &operator=(TimerResult &&) = default;

private:
	friend class TimerResultBuilder;
	friend class GeneratedObjectAccess;
	TimerResult();

public:
	// Deserialization
	static TimerResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	TimerResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string time_unit;
	int64_t count;
	int64_t total_duration;
};

class TimerResultBuilder {
public:
	TimerResultBuilder();
	TimerResultBuilder &SetTimeUnit(string value);
	TimerResultBuilder &SetCount(int64_t value);
	TimerResultBuilder &SetTotalDuration(int64_t value);
	string TryBuild(TimerResult &result);
	TimerResult Build();

private:
	TimerResult result_;
	bool has_time_unit_ = false;
	bool has_count_ = false;
	bool has_total_duration_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
