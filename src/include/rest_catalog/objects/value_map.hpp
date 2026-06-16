
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/integer_type_value.hpp"
#include "rest_catalog/objects/primitive_type_value.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ValueMapBuilder;

class ValueMap {
public:
	ValueMap(const ValueMap &);
	ValueMap &operator=(const ValueMap &) = delete;
	ValueMap(ValueMap &&);
	ValueMap &operator=(ValueMap &&) = delete;

private:
	friend class ValueMapBuilder;
	ValueMap(optional<vector<IntegerTypeValue>> keys_p, optional<vector<PrimitiveTypeValue>> values_p);

public:
	// Deserialization
	static ValueMap FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ValueMapBuilder &builder);
	string Validate() const;

	// Copy
	ValueMap Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<vector<IntegerTypeValue>> keys;
	const optional<vector<PrimitiveTypeValue>> values;
};

class ValueMapBuilder {
public:
	ValueMapBuilder();
	ValueMapBuilder &SetKeys(vector<IntegerTypeValue> value);
	ValueMapBuilder &SetValues(vector<PrimitiveTypeValue> value);
	string TryBuild(optional<ValueMap> &result);
	ValueMap Build();

private:
	optional<vector<IntegerTypeValue>> keys_;
	optional<vector<PrimitiveTypeValue>> values_;
};

} // namespace rest_api_objects
} // namespace duckdb
