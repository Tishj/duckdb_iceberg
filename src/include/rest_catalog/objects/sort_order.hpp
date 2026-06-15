
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/sort_field.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SortOrderBuilder;

class SortOrder {
public:
	SortOrder(const SortOrder &) = delete;
	SortOrder &operator=(const SortOrder &) = delete;
	SortOrder(SortOrder &&) = default;
	SortOrder &operator=(SortOrder &&) = default;

private:
	friend class SortOrderBuilder;
	friend class GeneratedObjectAccess;
	SortOrder();

public:
	// Deserialization
	static SortOrder FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SortOrder Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t order_id;
	vector<SortField> fields;
};

class SortOrderBuilder {
public:
	SortOrderBuilder();
	SortOrderBuilder &SetOrderId(int32_t value);
	SortOrderBuilder &SetFields(vector<SortField> value);
	string TryBuild(SortOrder &result);
	SortOrder Build();

private:
	SortOrder result_;
	bool has_order_id_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
