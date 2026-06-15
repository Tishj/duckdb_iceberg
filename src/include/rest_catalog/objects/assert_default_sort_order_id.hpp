
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/table_requirement_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AssertDefaultSortOrderId {
public:
	AssertDefaultSortOrderId();
	AssertDefaultSortOrderId(const AssertDefaultSortOrderId &) = delete;
	AssertDefaultSortOrderId &operator=(const AssertDefaultSortOrderId &) = delete;
	AssertDefaultSortOrderId(AssertDefaultSortOrderId &&) = default;
	AssertDefaultSortOrderId &operator=(AssertDefaultSortOrderId &&) = default;

public:
	// Deserialization
	static AssertDefaultSortOrderId FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(AssertDefaultSortOrderId &result);
	AssertDefaultSortOrderId Build();

private:
	AssertDefaultSortOrderId result_;
	bool has_type_ = false;
	bool has_default_sort_order_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
