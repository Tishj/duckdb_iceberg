
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/completed_planning_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CompletedPlanningWithIDResult {
public:
	CompletedPlanningWithIDResult();
	CompletedPlanningWithIDResult(const CompletedPlanningWithIDResult &) = delete;
	CompletedPlanningWithIDResult &operator=(const CompletedPlanningWithIDResult &) = delete;
	CompletedPlanningWithIDResult(CompletedPlanningWithIDResult &&) = default;
	CompletedPlanningWithIDResult &operator=(CompletedPlanningWithIDResult &&) = default;
	class Object6 {
	public:
		Object6();
		Object6(const Object6 &) = delete;
		Object6 &operator=(const Object6 &) = delete;
		Object6(Object6 &&) = default;
		Object6 &operator=(Object6 &&) = default;

	public:
		// Deserialization
		static Object6 FromJSON(yyjson_val *obj);
		string TryFromJSON(yyjson_val *obj);
		string Validate() const;

		// Copy
		Object6 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		string plan_id;
	};

	class Object6Builder {
	public:
		Object6Builder();
		Object6Builder &SetPlanId(string value);
		string TryBuild(Object6 &result);
		Object6 Build();

	private:
		Object6 result_;
		bool has_plan_id_ = false;
	};

public:
	// Deserialization
	static CompletedPlanningWithIDResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CompletedPlanningWithIDResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	CompletedPlanningResult completed_planning_result;
	Object6 object_6;
};

class CompletedPlanningWithIDResultBuilder {
public:
	CompletedPlanningWithIDResultBuilder();
	CompletedPlanningWithIDResultBuilder &SetCompletedPlanningResult(CompletedPlanningResult value);
	CompletedPlanningWithIDResultBuilder &SetObject6(CompletedPlanningWithIDResult::Object6 value);
	string TryBuild(CompletedPlanningWithIDResult &result);
	CompletedPlanningWithIDResult Build();

private:
	CompletedPlanningWithIDResult result_;
};

} // namespace rest_api_objects
} // namespace duckdb
