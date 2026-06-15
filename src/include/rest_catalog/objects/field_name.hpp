
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

class FieldName {
public:
	FieldName(const FieldName &) = delete;
	FieldName &operator=(const FieldName &) = delete;
	FieldName(FieldName &&) = default;
	FieldName &operator=(FieldName &&) = delete;
	FieldName(string value_p);

public:
	// Deserialization
	static FieldName FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<FieldName> &result);
	string Validate() const;

	// Copy
	FieldName Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string value;
};

} // namespace rest_api_objects
} // namespace duckdb
