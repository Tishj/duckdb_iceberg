
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

class LongTypeValue {
public:
	LongTypeValue(const LongTypeValue &);
	LongTypeValue &operator=(const LongTypeValue &) = delete;
	LongTypeValue(LongTypeValue &&);
	LongTypeValue &operator=(LongTypeValue &&) = delete;
	LongTypeValue(int64_t value_p);

public:
	// Deserialization
	static LongTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<LongTypeValue> &result);
	string Validate() const;

	// Copy
	LongTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int64_t value;
};

} // namespace rest_api_objects
} // namespace duckdb
