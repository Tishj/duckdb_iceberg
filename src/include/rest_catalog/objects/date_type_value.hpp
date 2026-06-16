
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

class DateTypeValue {
public:
	DateTypeValue(const DateTypeValue &);
	DateTypeValue &operator=(const DateTypeValue &) = delete;
	DateTypeValue(DateTypeValue &&);
	DateTypeValue &operator=(DateTypeValue &&) = delete;
	DateTypeValue(string value_p);

public:
	// Deserialization
	static DateTypeValue FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, optional<DateTypeValue> &result);
	optional<string> Validate() const;

	// Copy
	DateTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
