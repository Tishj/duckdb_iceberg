
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

class StructField;

class StructTypeBuilder;

class StructType {
public:
	StructType(const StructType &) = delete;
	StructType &operator=(const StructType &) = delete;
	StructType(StructType &&) = default;
	StructType &operator=(StructType &&) = default;

private:
	friend class StructTypeBuilder;
	friend class GeneratedObjectAccess;
	StructType();

public:
	// Deserialization
	static StructType FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	StructType Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	vector<unique_ptr<StructField>> fields;
};

class StructTypeBuilder {
public:
	StructTypeBuilder();
	StructTypeBuilder &SetType(string value);
	StructTypeBuilder &SetFields(vector<unique_ptr<StructField>> value);
	string TryBuild(StructType &result);
	StructType Build();

private:
	StructType result_;
	bool has_type_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
