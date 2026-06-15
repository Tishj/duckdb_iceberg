
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetDefaultSortOrderUpdate {
public:
	SetDefaultSortOrderUpdate();
	SetDefaultSortOrderUpdate(const SetDefaultSortOrderUpdate &) = delete;
	SetDefaultSortOrderUpdate &operator=(const SetDefaultSortOrderUpdate &) = delete;
	SetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate &&) = default;
	SetDefaultSortOrderUpdate &operator=(SetDefaultSortOrderUpdate &&) = default;

public:
	// Deserialization
	static SetDefaultSortOrderUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetDefaultSortOrderUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int32_t sort_order_id;
};

class SetDefaultSortOrderUpdateBuilder {
public:
	SetDefaultSortOrderUpdateBuilder();
	SetDefaultSortOrderUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetDefaultSortOrderUpdateBuilder &SetSortOrderId(int32_t value);
	string TryBuild(SetDefaultSortOrderUpdate &result);
	SetDefaultSortOrderUpdate Build();

private:
	SetDefaultSortOrderUpdate result_;
	bool has_sort_order_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
