
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/delete_file.hpp"
#include "rest_catalog/objects/file_scan_task.hpp"
#include "rest_catalog/objects/plan_task.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ScanTasksBuilder;

class ScanTasks {
public:
	ScanTasks(const ScanTasks &);
	ScanTasks &operator=(const ScanTasks &) = delete;
	ScanTasks(ScanTasks &&);
	ScanTasks &operator=(ScanTasks &&) = delete;

private:
	friend class ScanTasksBuilder;
	ScanTasks(optional<vector<DeleteFile>> delete_files_p, optional<vector<FileScanTask>> file_scan_tasks_p,
	          optional<vector<PlanTask>> plan_tasks_p);

public:
	// Deserialization
	static ScanTasks FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ScanTasksBuilder &builder);
	string Validate() const;

	// Copy
	ScanTasks Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<vector<DeleteFile>> delete_files;
	const optional<vector<FileScanTask>> file_scan_tasks;
	const optional<vector<PlanTask>> plan_tasks;
};

class ScanTasksBuilder {
public:
	ScanTasksBuilder();
	ScanTasksBuilder &SetDeleteFiles(vector<DeleteFile> value);
	ScanTasksBuilder &SetFileScanTasks(vector<FileScanTask> value);
	ScanTasksBuilder &SetPlanTasks(vector<PlanTask> value);
	string TryBuild(optional<ScanTasks> &result);
	ScanTasks Build();

private:
	optional<vector<DeleteFile>> delete_files_;
	optional<vector<FileScanTask>> file_scan_tasks_;
	optional<vector<PlanTask>> plan_tasks_;
};

} // namespace rest_api_objects
} // namespace duckdb
