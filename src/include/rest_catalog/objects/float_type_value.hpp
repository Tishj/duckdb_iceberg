
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

class FloatTypeValue {
public:
	FloatTypeValue(const FloatTypeValue &);
	FloatTypeValue &operator=(const FloatTypeValue &) = delete;
	FloatTypeValue(FloatTypeValue &&);
	FloatTypeValue &operator=(FloatTypeValue &&) = delete;
	FloatTypeValue(double value_p);

public:
	// Deserialization
	static FloatTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<FloatTypeValue> &result);
	string Validate() const;

	// Copy
	FloatTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const double value;
};

} // namespace rest_api_objects
} // namespace duckdb
