
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PlanTask {
public:
	PlanTask(const PlanTask &) = delete;
	PlanTask &operator=(const PlanTask &) = delete;
	PlanTask(PlanTask &&) = default;
	PlanTask &operator=(PlanTask &&) = delete;
	PlanTask(string value_p);

public:
	// Deserialization
	static PlanTask FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<PlanTask> &result);
	string Validate() const;

	// Copy
	PlanTask Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string value;
};

} // namespace rest_api_objects
} // namespace duckdb
