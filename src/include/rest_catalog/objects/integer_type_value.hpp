
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

class IntegerTypeValue {
public:
	IntegerTypeValue(const IntegerTypeValue &);
	IntegerTypeValue &operator=(const IntegerTypeValue &) = delete;
	IntegerTypeValue(IntegerTypeValue &&);
	IntegerTypeValue &operator=(IntegerTypeValue &&) = delete;
	IntegerTypeValue(int32_t value_p);

public:
	// Deserialization
	static IntegerTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<IntegerTypeValue> &result);
	string Validate() const;

	// Copy
	IntegerTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t value;
};

} // namespace rest_api_objects
} // namespace duckdb
