
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
	StructType &operator=(StructType &&) = delete;

private:
	friend class StructTypeBuilder;
	StructType(string type_p, vector<unique_ptr<StructField>> fields_p);

public:
	// Deserialization
	static StructType FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, StructTypeBuilder &builder);
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
	string TryBuild(optional<StructType> &result);
	StructType Build();

private:
	optional<string> type_;
	optional<vector<unique_ptr<StructField>>> fields_;
	bool has_type_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
