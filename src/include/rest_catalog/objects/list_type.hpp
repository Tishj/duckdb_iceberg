
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

class ListTypeBuilder;

class ListType {
public:
	ListType(const ListType &) = delete;
	ListType &operator=(const ListType &) = delete;
	ListType(ListType &&) = default;
	ListType &operator=(ListType &&) = default;

private:
	friend class ListTypeBuilder;
	friend class GeneratedObjectAccess;
	ListType();

public:
	// Deserialization
	static ListType FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	ListType Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	int32_t element_id;
	unique_ptr<Type> element;
	bool element_required;
};

class ListTypeBuilder {
public:
	ListTypeBuilder();
	ListTypeBuilder &SetType(string value);
	ListTypeBuilder &SetElementId(int32_t value);
	ListTypeBuilder &SetElement(unique_ptr<Type> value);
	ListTypeBuilder &SetElementRequired(bool value);
	string TryBuild(ListType &result);
	ListType Build();

private:
	ListType result_;
	bool has_type_ = false;
	bool has_element_id_ = false;
	bool has_element_ = false;
	bool has_element_required_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
