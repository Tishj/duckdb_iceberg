
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/iceberg_error_response.hpp"
#include "rest_catalog/objects/plan_status.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class FailedPlanningResult {
public:
	FailedPlanningResult();
	FailedPlanningResult(const FailedPlanningResult &) = delete;
	FailedPlanningResult &operator=(const FailedPlanningResult &) = delete;
	FailedPlanningResult(FailedPlanningResult &&) = default;
	FailedPlanningResult &operator=(FailedPlanningResult &&) = default;
	class Object7 {
	public:
		Object7();
		Object7(const Object7 &) = delete;
		Object7 &operator=(const Object7 &) = delete;
		Object7(Object7 &&) = default;
		Object7 &operator=(Object7 &&) = default;

	public:
		// Deserialization
		static Object7 FromJSON(yyjson_val *obj);
		string TryFromJSON(yyjson_val *obj);
		string Validate() const;

		// Copy
		Object7 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		PlanStatus status;
	};

	class Object7Builder {
	public:
		Object7Builder();
		Object7Builder &SetStatus(PlanStatus value);
		string TryBuild(Object7 &result);
		Object7 Build();

	private:
		Object7 result_;
		bool has_status_ = false;
	};

public:
	// Deserialization
	static FailedPlanningResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	FailedPlanningResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	IcebergErrorResponse iceberg_error_response;
	Object7 object_7;
};

class FailedPlanningResultBuilder {
public:
	FailedPlanningResultBuilder();
	FailedPlanningResultBuilder &SetIcebergErrorResponse(IcebergErrorResponse value);
	FailedPlanningResultBuilder &SetObject7(FailedPlanningResult::Object7 value);
	string TryBuild(FailedPlanningResult &result);
	FailedPlanningResult Build();

private:
	FailedPlanningResult result_;
};

} // namespace rest_api_objects
} // namespace duckdb
