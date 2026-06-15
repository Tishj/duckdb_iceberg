
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/list_type.hpp"
#include "rest_catalog/objects/map_type.hpp"
#include "rest_catalog/objects/primitive_type.hpp"
#include "rest_catalog/objects/struct_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TypeBuilder;

class Type {
public:
	Type(const Type &) = delete;
	Type &operator=(const Type &) = delete;
	Type(Type &&) = default;
	Type &operator=(Type &&) = delete;

private:
	friend class TypeBuilder;
	Type(optional<PrimitiveType> primitive_type_p, optional<StructType> struct_type_p, optional<ListType> list_type_p,
	     optional<MapType> map_type_p);

public:
	// Deserialization
	static Type FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<Type> &result);
	string Validate() const;

	// Copy
	Type Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<PrimitiveType> primitive_type;
	optional<StructType> struct_type;
	optional<ListType> list_type;
	optional<MapType> map_type;
};

class TypeBuilder {
public:
	TypeBuilder();
	TypeBuilder &SetPrimitiveType(PrimitiveType value);
	TypeBuilder &SetStructType(StructType value);
	TypeBuilder &SetListType(ListType value);
	TypeBuilder &SetMapType(MapType value);
	string TryBuild(optional<Type> &result);
	Type Build();

private:
	optional<PrimitiveType> primitive_type_;
	optional<StructType> struct_type_;
	optional<ListType> list_type_;
	optional<MapType> map_type_;
};

} // namespace rest_api_objects
} // namespace duckdb
