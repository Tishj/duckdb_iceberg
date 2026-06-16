
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

class Type;

class MapTypeBuilder;

class MapType {
public:
	MapType(const MapType &);
	MapType &operator=(const MapType &) = delete;
	MapType(MapType &&);
	MapType &operator=(MapType &&) = delete;

private:
	friend class MapTypeBuilder;
	MapType(string type_p, int32_t key_id_p, unique_ptr<Type> key_p, int32_t value_id_p, unique_ptr<Type> value_p,
	        bool value_required_p);

public:
	// Deserialization
	static MapType FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, MapTypeBuilder &builder);
	optional<string> Validate() const;

	// Copy
	MapType Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string type;
	const int32_t key_id;
	const unique_ptr<Type> key;
	const int32_t value_id;
	const unique_ptr<Type> value;
	const bool value_required;
};

class MapTypeBuilder {
public:
	MapTypeBuilder();
	MapTypeBuilder &SetType(string value);
	MapTypeBuilder &SetKeyId(int32_t value);
	MapTypeBuilder &SetKey(unique_ptr<Type> value);
	MapTypeBuilder &SetValueId(int32_t value);
	MapTypeBuilder &SetValue(unique_ptr<Type> value);
	MapTypeBuilder &SetValueRequired(bool value);
	optional<string> TryBuild(optional<MapType> &result);
	MapType Build();

private:
	optional<string> type_;
	optional<int32_t> key_id_;
	unique_ptr<Type> key_;
	optional<int32_t> value_id_;
	unique_ptr<Type> value_;
	optional<bool> value_required_;
	bool has_type_ = false;
	bool has_key_id_ = false;
	bool has_key_ = false;
	bool has_value_id_ = false;
	bool has_value_ = false;
	bool has_value_required_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
