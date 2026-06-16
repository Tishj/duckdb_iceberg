
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_requirement_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AssertDefaultSortOrderIdBuilder;

class AssertDefaultSortOrderId {
public:
	AssertDefaultSortOrderId(const AssertDefaultSortOrderId &) = delete;
	AssertDefaultSortOrderId &operator=(const AssertDefaultSortOrderId &) = delete;
	AssertDefaultSortOrderId(AssertDefaultSortOrderId &&) = default;
	AssertDefaultSortOrderId &operator=(AssertDefaultSortOrderId &&) = delete;

private:
	friend class AssertDefaultSortOrderIdBuilder;
	AssertDefaultSortOrderId(TableRequirementType type_p, int32_t default_sort_order_id_p);

public:
	// Deserialization
	static AssertDefaultSortOrderId FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertDefaultSortOrderIdBuilder &builder);
	string Validate() const;

	// Copy
	AssertDefaultSortOrderId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	TableRequirementType type;
	int32_t default_sort_order_id;
};

class AssertDefaultSortOrderIdBuilder {
public:
	AssertDefaultSortOrderIdBuilder();
	AssertDefaultSortOrderIdBuilder &SetType(TableRequirementType value);
	AssertDefaultSortOrderIdBuilder &SetDefaultSortOrderId(int32_t value);
	string TryBuild(optional<AssertDefaultSortOrderId> &result);
	AssertDefaultSortOrderId Build();

private:
	optional<TableRequirementType> type_;
	optional<int32_t> default_sort_order_id_;
	bool has_type_ = false;
	bool has_default_sort_order_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
