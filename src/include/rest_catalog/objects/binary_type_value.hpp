
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

class BinaryTypeValue {
public:
	BinaryTypeValue(const BinaryTypeValue &);
	BinaryTypeValue &operator=(const BinaryTypeValue &) = delete;
	BinaryTypeValue(BinaryTypeValue &&);
	BinaryTypeValue &operator=(BinaryTypeValue &&) = delete;
	BinaryTypeValue(string value_p);

public:
	// Deserialization
	static BinaryTypeValue FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<BinaryTypeValue> &result);
	string Validate() const;

	// Copy
	BinaryTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
