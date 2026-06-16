
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

class DoubleTypeValue {
public:
	DoubleTypeValue(const DoubleTypeValue &);
	DoubleTypeValue &operator=(const DoubleTypeValue &) = delete;
	DoubleTypeValue(DoubleTypeValue &&);
	DoubleTypeValue &operator=(DoubleTypeValue &&) = delete;
	DoubleTypeValue(double value_p);

public:
	// Deserialization
	static DoubleTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<DoubleTypeValue> &result);
	string Validate() const;

	// Copy
	DoubleTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const double value;
};

} // namespace rest_api_objects
} // namespace duckdb
