
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

class TimestampTypeValue {
public:
	TimestampTypeValue(const TimestampTypeValue &);
	TimestampTypeValue &operator=(const TimestampTypeValue &) = delete;
	TimestampTypeValue(TimestampTypeValue &&);
	TimestampTypeValue &operator=(TimestampTypeValue &&) = delete;
	TimestampTypeValue(string value_p);

public:
	// Deserialization
	static TimestampTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<TimestampTypeValue> &result);
	string Validate() const;

	// Copy
	TimestampTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
