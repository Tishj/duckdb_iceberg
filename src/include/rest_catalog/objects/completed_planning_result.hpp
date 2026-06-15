
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/plan_status.hpp"
#include "rest_catalog/objects/scan_tasks.hpp"
#include "rest_catalog/objects/storage_credential.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CompletedPlanningResultBuilder;

class CompletedPlanningResult {
public:
	CompletedPlanningResult(const CompletedPlanningResult &) = delete;
	CompletedPlanningResult &operator=(const CompletedPlanningResult &) = delete;
	CompletedPlanningResult(CompletedPlanningResult &&) = default;
	CompletedPlanningResult &operator=(CompletedPlanningResult &&) = delete;
	class Object5Builder;

	class Object5 {
	public:
		Object5(const Object5 &) = delete;
		Object5 &operator=(const Object5 &) = delete;
		Object5(Object5 &&) = default;
		Object5 &operator=(Object5 &&) = delete;

	private:
		friend class Object5Builder;
		Object5(PlanStatus status_p, optional<vector<StorageCredential>> storage_credentials_p);

	public:
		// Deserialization
		static Object5 FromJSON(yyjson_val *obj);
		static string TryFromJSON(yyjson_val *obj, optional<Object5> &result);
		string Validate() const;

		// Copy
		Object5 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		PlanStatus status;
		optional<vector<StorageCredential>> storage_credentials;
	};

	class Object5Builder {
	public:
		Object5Builder();
		Object5Builder &SetStatus(PlanStatus value);
		Object5Builder &SetStorageCredentials(vector<StorageCredential> value);
		string TryBuild(optional<Object5> &result);
		Object5 Build();

	private:
		optional<PlanStatus> status_;
		optional<vector<StorageCredential>> storage_credentials_;
		bool has_status_ = false;
	};

private:
	friend class CompletedPlanningResultBuilder;
	CompletedPlanningResult(ScanTasks scan_tasks_p, Object5 object_5_p);

public:
	// Deserialization
	static CompletedPlanningResult FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<CompletedPlanningResult> &result);
	string Validate() const;

	// Copy
	CompletedPlanningResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ScanTasks scan_tasks;
	Object5 object_5;
};

class CompletedPlanningResultBuilder {
public:
	CompletedPlanningResultBuilder();
	CompletedPlanningResultBuilder &SetScanTasks(ScanTasks value);
	CompletedPlanningResultBuilder &SetObject5(CompletedPlanningResult::Object5 value);
	string TryBuild(optional<CompletedPlanningResult> &result);
	CompletedPlanningResult Build();

private:
	optional<ScanTasks> scan_tasks_;
	optional<CompletedPlanningResult::Object5> object_5_;
};

} // namespace rest_api_objects
} // namespace duckdb
