
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
	StructField(const StructField &);
	StructField &operator=(const StructField &) = delete;
	StructField(StructField &&);
	StructField &operator=(StructField &&) = delete;

private:
	friend class StructFieldBuilder;
	StructField(int32_t id_p, string name_p, unique_ptr<Type> type_p, bool required_p, optional<string> _doc_p,
	            optional<PrimitiveTypeValue> initial_default_p, optional<PrimitiveTypeValue> write_default_p);

public:
	// Deserialization
	static StructField FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, StructFieldBuilder &builder);
	string Validate() const;

	// Copy
	StructField Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t id;
	const string name;
	const unique_ptr<Type> type;
	const bool required;
	const optional<string> _doc;
	const optional<PrimitiveTypeValue> initial_default;
	const optional<PrimitiveTypeValue> write_default;
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
	string TryBuild(optional<StructField> &result);
	StructField Build();

private:
	optional<int32_t> id_;
	optional<string> name_;
	unique_ptr<Type> type_;
	optional<bool> required_;
	optional<string> _doc_;
	optional<PrimitiveTypeValue> initial_default_;
	optional<PrimitiveTypeValue> write_default_;
	bool has_id_ = false;
	bool has_name_ = false;
	bool has_type_ = false;
	bool has_required_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
