
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/completed_planning_result.hpp"
#include "rest_catalog/objects/empty_planning_result.hpp"
#include "rest_catalog/objects/failed_planning_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class FetchPlanningResultBuilder;

class FetchPlanningResult {
public:
	FetchPlanningResult(const FetchPlanningResult &) = delete;
	FetchPlanningResult &operator=(const FetchPlanningResult &) = delete;
	FetchPlanningResult(FetchPlanningResult &&) = default;
	FetchPlanningResult &operator=(FetchPlanningResult &&) = default;

private:
	friend class FetchPlanningResultBuilder;
	friend class GeneratedObjectAccess;
	FetchPlanningResult();

public:
	// Deserialization
	static FetchPlanningResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	FetchPlanningResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<CompletedPlanningResult> completed_planning_result;
	optional<FailedPlanningResult> failed_planning_result;
	optional<EmptyPlanningResult> empty_planning_result;
};

class FetchPlanningResultBuilder {
public:
	FetchPlanningResultBuilder();
	FetchPlanningResultBuilder &SetCompletedPlanningResult(CompletedPlanningResult value);
	FetchPlanningResultBuilder &SetFailedPlanningResult(FailedPlanningResult value);
	FetchPlanningResultBuilder &SetEmptyPlanningResult(EmptyPlanningResult value);
	string TryBuild(FetchPlanningResult &result);
	FetchPlanningResult Build();

private:
	FetchPlanningResult result_;
};

} // namespace rest_api_objects
} // namespace duckdb
