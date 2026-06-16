
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

class PlanStatus {
public:
	PlanStatus(const PlanStatus &);
	PlanStatus &operator=(const PlanStatus &) = delete;
	PlanStatus(PlanStatus &&);
	PlanStatus &operator=(PlanStatus &&) = delete;
	PlanStatus(string value_p);

public:
	// Deserialization
	static PlanStatus FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, optional<PlanStatus> &result);
	optional<string> Validate() const;

	// Copy
	PlanStatus Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
