
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

class TimeTypeValue {
public:
	TimeTypeValue(const TimeTypeValue &) = delete;
	TimeTypeValue &operator=(const TimeTypeValue &) = delete;
	TimeTypeValue(TimeTypeValue &&) = default;
	TimeTypeValue &operator=(TimeTypeValue &&) = default;
	TimeTypeValue();

public:
	// Deserialization
	static TimeTypeValue FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	TimeTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string value;
};

} // namespace rest_api_objects
} // namespace duckdb
