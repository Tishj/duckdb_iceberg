
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/primitive_type_value.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class Type;

class StructFieldBuilder;

class StructField {
public:
	StructField(const StructField &) = delete;
	StructField &operator=(const StructField &) = delete;
	StructField(StructField &&) = default;
	StructField &operator=(StructField &&) = default;

private:
	friend class StructFieldBuilder;
	friend class GeneratedObjectAccess;
	StructField();

public:
	// Deserialization
	static StructField FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	StructField Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t id;
	string name;
	unique_ptr<Type> type;
	bool required;
	optional<string> _doc;
	optional<PrimitiveTypeValue> initial_default;
	optional<PrimitiveTypeValue> write_default;
};

class StructFieldBuilder {
public:
	StructFieldBuilder();
	StructFieldBuilder &SetId(int32_t value);
	StructFieldBuilder &SetName(string value);
	StructFieldBuilder &SetType(unique_ptr<Type> value);
	StructFieldBuilder &SetRequired(bool value);
	StructFieldBuilder &SetDoc(string value);
	StructFieldBuilder &SetInitialDefault(PrimitiveTypeValue value);
	StructFieldBuilder &SetWriteDefault(PrimitiveTypeValue value);
	string TryBuild(StructField &result);
	StructField Build();

private:
	StructField result_;
	bool has_id_ = false;
	bool has_name_ = false;
	bool has_type_ = false;
	bool has_required_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
