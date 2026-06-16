
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

class EmptyPlanningResultBuilder;

class EmptyPlanningResult {
public:
	EmptyPlanningResult(const EmptyPlanningResult &);
	EmptyPlanningResult &operator=(const EmptyPlanningResult &) = delete;
	EmptyPlanningResult(EmptyPlanningResult &&);
	EmptyPlanningResult &operator=(EmptyPlanningResult &&) = delete;

private:
	friend class EmptyPlanningResultBuilder;
	EmptyPlanningResult(PlanStatus status_p);

public:
	// Deserialization
	static EmptyPlanningResult FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, EmptyPlanningResultBuilder &builder);
	optional<string> Validate() const;

	// Copy
	EmptyPlanningResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const PlanStatus status;
};

class EmptyPlanningResultBuilder {
public:
	EmptyPlanningResultBuilder();
	EmptyPlanningResultBuilder &SetStatus(PlanStatus value);
	optional<string> TryBuild(optional<EmptyPlanningResult> &result);
	EmptyPlanningResult Build();

private:
	optional<PlanStatus> status_;
	bool has_status_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
