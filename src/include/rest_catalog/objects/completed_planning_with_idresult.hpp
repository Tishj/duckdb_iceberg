
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/completed_planning_result.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CompletedPlanningWithIDResultBuilder;

class CompletedPlanningWithIDResult {
public:
	CompletedPlanningWithIDResult(const CompletedPlanningWithIDResult &);
	CompletedPlanningWithIDResult &operator=(const CompletedPlanningWithIDResult &) = delete;
	CompletedPlanningWithIDResult(CompletedPlanningWithIDResult &&);
	CompletedPlanningWithIDResult &operator=(CompletedPlanningWithIDResult &&) = delete;
	class Object6Builder;

	class Object6 {
	public:
		Object6(const Object6 &);
		Object6 &operator=(const Object6 &) = delete;
		Object6(Object6 &&);
		Object6 &operator=(Object6 &&) = delete;

	private:
		friend class Object6Builder;
		Object6(string plan_id_p);

	public:
		// Deserialization
		static Object6 FromJSON(yyjson_val *obj);
		static optional<string> TryFromJSON(yyjson_val *obj, Object6Builder &builder);
		optional<string> Validate() const;

		// Copy
		Object6 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		const string plan_id;
	};

	class Object6Builder {
	public:
		Object6Builder();
		Object6Builder &SetPlanId(string value);
		optional<string> TryBuild(optional<Object6> &result);
		Object6 Build();

	private:
		optional<string> plan_id_;
		bool has_plan_id_ = false;
	};

private:
	friend class CompletedPlanningWithIDResultBuilder;
	CompletedPlanningWithIDResult(CompletedPlanningResult completed_planning_result_p, Object6 object_6_p);

public:
	// Deserialization
	static CompletedPlanningWithIDResult FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CompletedPlanningWithIDResultBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CompletedPlanningWithIDResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const CompletedPlanningResult completed_planning_result;
	const Object6 object_6;
};

class CompletedPlanningWithIDResultBuilder {
public:
	CompletedPlanningWithIDResultBuilder();
	CompletedPlanningWithIDResultBuilder &SetCompletedPlanningResult(CompletedPlanningResult value);
	CompletedPlanningWithIDResultBuilder &SetObject6(CompletedPlanningWithIDResult::Object6 value);
	optional<string> TryBuild(optional<CompletedPlanningWithIDResult> &result);
	CompletedPlanningWithIDResult Build();

private:
	optional<CompletedPlanningResult> completed_planning_result_;
	optional<CompletedPlanningWithIDResult::Object6> object_6_;
};

} // namespace rest_api_objects
} // namespace duckdb
