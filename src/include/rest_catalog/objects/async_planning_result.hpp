
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/plan_status.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AsyncPlanningResultBuilder;

class AsyncPlanningResult {
public:
	AsyncPlanningResult(const AsyncPlanningResult &) = delete;
	AsyncPlanningResult &operator=(const AsyncPlanningResult &) = delete;
	AsyncPlanningResult(AsyncPlanningResult &&) = default;
	AsyncPlanningResult &operator=(AsyncPlanningResult &&) = delete;

private:
	friend class AsyncPlanningResultBuilder;
	AsyncPlanningResult(PlanStatus status_p, string plan_id_p);

public:
	// Deserialization
	static AsyncPlanningResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<AsyncPlanningResult> &result);
	string Validate() const;

	// Copy
	AsyncPlanningResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	PlanStatus status;
	string plan_id;
};

class AsyncPlanningResultBuilder {
public:
	AsyncPlanningResultBuilder();
	AsyncPlanningResultBuilder &SetStatus(PlanStatus value);
	AsyncPlanningResultBuilder &SetPlanId(string value);
	string TryBuild(optional<AsyncPlanningResult> &result);
	AsyncPlanningResult Build();

private:
	optional<PlanStatus> status_;
	optional<string> plan_id_;
	bool has_status_ = false;
	bool has_plan_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
