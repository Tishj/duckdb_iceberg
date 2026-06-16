
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/sort_order.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddSortOrderUpdateBuilder;

class AddSortOrderUpdate {
public:
	AddSortOrderUpdate(const AddSortOrderUpdate &);
	AddSortOrderUpdate &operator=(const AddSortOrderUpdate &) = delete;
	AddSortOrderUpdate(AddSortOrderUpdate &&);
	AddSortOrderUpdate &operator=(AddSortOrderUpdate &&) = delete;

private:
	friend class AddSortOrderUpdateBuilder;
	AddSortOrderUpdate(BaseUpdate base_update_p, SortOrder sort_order_p);

public:
	// Deserialization
	static AddSortOrderUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AddSortOrderUpdateBuilder &builder);
	string Validate() const;

	// Copy
	AddSortOrderUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const SortOrder sort_order;
};

class AddSortOrderUpdateBuilder {
public:
	AddSortOrderUpdateBuilder();
	AddSortOrderUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddSortOrderUpdateBuilder &SetSortOrder(SortOrder value);
	string TryBuild(optional<AddSortOrderUpdate> &result);
	AddSortOrderUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<SortOrder> sort_order_;
	bool has_sort_order_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
