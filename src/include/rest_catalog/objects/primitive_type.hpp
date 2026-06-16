
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

class PrimitiveType {
public:
	PrimitiveType(const PrimitiveType &);
	PrimitiveType &operator=(const PrimitiveType &) = delete;
	PrimitiveType(PrimitiveType &&);
	PrimitiveType &operator=(PrimitiveType &&) = delete;
	PrimitiveType(string value_p);

public:
	// Deserialization
	static PrimitiveType FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, optional<PrimitiveType> &result);
	optional<string> Validate() const;

	// Copy
	PrimitiveType Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
