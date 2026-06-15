
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/integer_type_value.hpp"
#include "rest_catalog/objects/long_type_value.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CountMapBuilder;

class CountMap {
public:
	CountMap(const CountMap &) = delete;
	CountMap &operator=(const CountMap &) = delete;
	CountMap(CountMap &&) = default;
	CountMap &operator=(CountMap &&) = default;

private:
	friend class CountMapBuilder;
	friend class GeneratedObjectAccess;
	CountMap();

public:
	// Deserialization
	static CountMap FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CountMap Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<vector<IntegerTypeValue>> keys;
	optional<vector<LongTypeValue>> values;
};

class CountMapBuilder {
public:
	CountMapBuilder();
	CountMapBuilder &SetKeys(vector<IntegerTypeValue> value);
	CountMapBuilder &SetValues(vector<LongTypeValue> value);
	string TryBuild(CountMap &result);
	CountMap Build();

private:
	CountMap result_;
};

} // namespace rest_api_objects
} // namespace duckdb
