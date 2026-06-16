
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetDefaultSortOrderUpdateBuilder;

class SetDefaultSortOrderUpdate {
public:
	SetDefaultSortOrderUpdate(const SetDefaultSortOrderUpdate &);
	SetDefaultSortOrderUpdate &operator=(const SetDefaultSortOrderUpdate &) = delete;
	SetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate &&);
	SetDefaultSortOrderUpdate &operator=(SetDefaultSortOrderUpdate &&) = delete;

private:
	friend class SetDefaultSortOrderUpdateBuilder;
	SetDefaultSortOrderUpdate(BaseUpdate base_update_p, int32_t sort_order_id_p);

public:
	// Deserialization
	static SetDefaultSortOrderUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SetDefaultSortOrderUpdateBuilder &builder);
	string Validate() const;

	// Copy
	SetDefaultSortOrderUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const int32_t sort_order_id;
};

class SetDefaultSortOrderUpdateBuilder {
public:
	SetDefaultSortOrderUpdateBuilder();
	SetDefaultSortOrderUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetDefaultSortOrderUpdateBuilder &SetSortOrderId(int32_t value);
	string TryBuild(optional<SetDefaultSortOrderUpdate> &result);
	SetDefaultSortOrderUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int32_t> sort_order_id_;
	bool has_sort_order_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
