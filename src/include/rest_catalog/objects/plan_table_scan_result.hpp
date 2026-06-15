
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/async_planning_result.hpp"
#include "rest_catalog/objects/completed_planning_with_idresult.hpp"
#include "rest_catalog/objects/empty_planning_result.hpp"
#include "rest_catalog/objects/failed_planning_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PlanTableScanResultBuilder;

class PlanTableScanResult {
public:
	PlanTableScanResult(const PlanTableScanResult &) = delete;
	PlanTableScanResult &operator=(const PlanTableScanResult &) = delete;
	PlanTableScanResult(PlanTableScanResult &&) = default;
	PlanTableScanResult &operator=(PlanTableScanResult &&) = default;

private:
	friend class PlanTableScanResultBuilder;
	friend class GeneratedObjectAccess;
	PlanTableScanResult();

public:
	// Deserialization
	static PlanTableScanResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	PlanTableScanResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<CompletedPlanningWithIDResult> completed_planning_with_idresult;
	optional<FailedPlanningResult> failed_planning_result;
	optional<AsyncPlanningResult> async_planning_result;
	optional<EmptyPlanningResult> empty_planning_result;
};

class PlanTableScanResultBuilder {
public:
	PlanTableScanResultBuilder();
	PlanTableScanResultBuilder &SetCompletedPlanningWithIdresult(CompletedPlanningWithIDResult value);
	PlanTableScanResultBuilder &SetFailedPlanningResult(FailedPlanningResult value);
	PlanTableScanResultBuilder &SetAsyncPlanningResult(AsyncPlanningResult value);
	PlanTableScanResultBuilder &SetEmptyPlanningResult(EmptyPlanningResult value);
	string TryBuild(PlanTableScanResult &result);
	PlanTableScanResult Build();

private:
	PlanTableScanResult result_;
};

} // namespace rest_api_objects
} // namespace duckdb
