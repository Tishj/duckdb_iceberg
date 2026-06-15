
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/scan_tasks.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class FetchScanTasksResultBuilder;

class FetchScanTasksResult {
public:
	FetchScanTasksResult(const FetchScanTasksResult &) = delete;
	FetchScanTasksResult &operator=(const FetchScanTasksResult &) = delete;
	FetchScanTasksResult(FetchScanTasksResult &&) = default;
	FetchScanTasksResult &operator=(FetchScanTasksResult &&) = delete;

private:
	friend class FetchScanTasksResultBuilder;
	FetchScanTasksResult(ScanTasks scan_tasks_p);

public:
	// Deserialization
	static FetchScanTasksResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<FetchScanTasksResult> &result);
	string Validate() const;

	// Copy
	FetchScanTasksResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ScanTasks scan_tasks;
};

class FetchScanTasksResultBuilder {
public:
	FetchScanTasksResultBuilder();
	FetchScanTasksResultBuilder &SetScanTasks(ScanTasks value);
	string TryBuild(optional<FetchScanTasksResult> &result);
	FetchScanTasksResult Build();

private:
	optional<ScanTasks> scan_tasks_;
};

} // namespace rest_api_objects
} // namespace duckdb
