
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
	CountMap(const CountMap &);
	CountMap &operator=(const CountMap &) = delete;
	CountMap(CountMap &&);
	CountMap &operator=(CountMap &&) = delete;

private:
	friend class CountMapBuilder;
	CountMap(optional<vector<IntegerTypeValue>> keys_p, optional<vector<LongTypeValue>> values_p);

public:
	// Deserialization
	static CountMap FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, CountMapBuilder &builder);
	string Validate() const;

	// Copy
	CountMap Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<vector<IntegerTypeValue>> keys;
	const optional<vector<LongTypeValue>> values;
};

class CountMapBuilder {
public:
	CountMapBuilder();
	CountMapBuilder &SetKeys(vector<IntegerTypeValue> value);
	CountMapBuilder &SetValues(vector<LongTypeValue> value);
	string TryBuild(optional<CountMap> &result);
	CountMap Build();

private:
	optional<vector<IntegerTypeValue>> keys_;
	optional<vector<LongTypeValue>> values_;
};

} // namespace rest_api_objects
} // namespace duckdb
