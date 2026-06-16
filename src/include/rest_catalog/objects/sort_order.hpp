
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
	SortOrder(const SortOrder &);
	SortOrder &operator=(const SortOrder &) = delete;
	SortOrder(SortOrder &&);
	SortOrder &operator=(SortOrder &&) = delete;

private:
	friend class SortOrderBuilder;
	SortOrder(int32_t order_id_p, vector<SortField> fields_p);

public:
	// Deserialization
	static SortOrder FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SortOrderBuilder &builder);
	string Validate() const;

	// Copy
	SortOrder Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t order_id;
	const vector<SortField> fields;
};

class SortOrderBuilder {
public:
	SortOrderBuilder();
	SortOrderBuilder &SetOrderId(int32_t value);
	SortOrderBuilder &SetFields(vector<SortField> value);
	string TryBuild(optional<SortOrder> &result);
	SortOrder Build();

private:
	optional<int32_t> order_id_;
	optional<vector<SortField>> fields_;
	bool has_order_id_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
