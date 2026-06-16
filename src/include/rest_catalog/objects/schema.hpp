
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/struct_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SchemaBuilder;

class Schema {
public:
	Schema(const Schema &);
	Schema &operator=(const Schema &) = delete;
	Schema(Schema &&);
	Schema &operator=(Schema &&) = delete;
	class Object1Builder;

	class Object1 {
	public:
		Object1(const Object1 &);
		Object1 &operator=(const Object1 &) = delete;
		Object1(Object1 &&);
		Object1 &operator=(Object1 &&) = delete;

	private:
		friend class Object1Builder;
		Object1(optional<int32_t> schema_id_p, optional<vector<int32_t>> identifier_field_ids_p);

	public:
		// Deserialization
		static Object1 FromJSON(yyjson_val *obj);
		static string TryFromJSON(yyjson_val *obj, Object1Builder &builder);
		string Validate() const;

		// Copy
		Object1 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		const optional<int32_t> schema_id;
		const optional<vector<int32_t>> identifier_field_ids;
	};

	class Object1Builder {
	public:
		Object1Builder();
		Object1Builder &SetSchemaId(int32_t value);
		Object1Builder &SetIdentifierFieldIds(vector<int32_t> value);
		string TryBuild(optional<Object1> &result);
		Object1 Build();

	private:
		optional<int32_t> schema_id_;
		optional<vector<int32_t>> identifier_field_ids_;
	};

private:
	friend class SchemaBuilder;
	Schema(StructType struct_type_p, Object1 object_1_p);

public:
	// Deserialization
	static Schema FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SchemaBuilder &builder);
	string Validate() const;

	// Copy
	Schema Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const StructType struct_type;
	const Object1 object_1;
};

class SchemaBuilder {
public:
	SchemaBuilder();
	SchemaBuilder &SetStructType(StructType value);
	SchemaBuilder &SetObject1(Schema::Object1 value);
	string TryBuild(optional<Schema> &result);
	Schema Build();

private:
	optional<StructType> struct_type_;
	optional<Schema::Object1> object_1_;
};

} // namespace rest_api_objects
} // namespace duckdb
