
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/plan_task.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class FetchScanTasksRequestBuilder;

class FetchScanTasksRequest {
public:
	FetchScanTasksRequest(const FetchScanTasksRequest &) = delete;
	FetchScanTasksRequest &operator=(const FetchScanTasksRequest &) = delete;
	FetchScanTasksRequest(FetchScanTasksRequest &&) = default;
	FetchScanTasksRequest &operator=(FetchScanTasksRequest &&) = default;

private:
	friend class FetchScanTasksRequestBuilder;
	friend class GeneratedObjectAccess;
	FetchScanTasksRequest();

public:
	// Deserialization
	static FetchScanTasksRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	FetchScanTasksRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	PlanTask plan_task;
};

class FetchScanTasksRequestBuilder {
public:
	FetchScanTasksRequestBuilder();
	FetchScanTasksRequestBuilder &SetPlanTask(PlanTask value);
	string TryBuild(FetchScanTasksRequest &result);
	FetchScanTasksRequest Build();

private:
	FetchScanTasksRequest result_;
	bool has_plan_task_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
