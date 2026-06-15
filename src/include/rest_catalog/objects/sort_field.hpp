
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/null_order.hpp"
#include "rest_catalog/objects/sort_direction.hpp"
#include "rest_catalog/objects/transform.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SortFieldBuilder;

class SortField {
public:
	SortField(const SortField &) = delete;
	SortField &operator=(const SortField &) = delete;
	SortField(SortField &&) = default;
	SortField &operator=(SortField &&) = delete;

private:
	friend class SortFieldBuilder;
	SortField(int32_t source_id_p, Transform transform_p, SortDirection direction_p, NullOrder null_order_p);

public:
	// Deserialization
	static SortField FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<SortField> &result);
	string Validate() const;

	// Copy
	SortField Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t source_id;
	Transform transform;
	SortDirection direction;
	NullOrder null_order;
};

class SortFieldBuilder {
public:
	SortFieldBuilder();
	SortFieldBuilder &SetSourceId(int32_t value);
	SortFieldBuilder &SetTransform(Transform value);
	SortFieldBuilder &SetDirection(SortDirection value);
	SortFieldBuilder &SetNullOrder(NullOrder value);
	string TryBuild(optional<SortField> &result);
	SortField Build();

private:
	optional<int32_t> source_id_;
	optional<Transform> transform_;
	optional<SortDirection> direction_;
	optional<NullOrder> null_order_;
	bool has_source_id_ = false;
	bool has_transform_ = false;
	bool has_direction_ = false;
	bool has_null_order_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
