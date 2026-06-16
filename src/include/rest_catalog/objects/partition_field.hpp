
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/transform.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PartitionFieldBuilder;

class PartitionField {
public:
	PartitionField(const PartitionField &) = delete;
	PartitionField &operator=(const PartitionField &) = delete;
	PartitionField(PartitionField &&) = default;
	PartitionField &operator=(PartitionField &&) = delete;

private:
	friend class PartitionFieldBuilder;
	PartitionField(int32_t source_id_p, Transform transform_p, string name_p, optional<int32_t> field_id_p);

public:
	// Deserialization
	static PartitionField FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, PartitionFieldBuilder &builder);
	string Validate() const;

	// Copy
	PartitionField Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t source_id;
	Transform transform;
	string name;
	optional<int32_t> field_id;
};

class PartitionFieldBuilder {
public:
	PartitionFieldBuilder();
	PartitionFieldBuilder &SetSourceId(int32_t value);
	PartitionFieldBuilder &SetTransform(Transform value);
	PartitionFieldBuilder &SetName(string value);
	PartitionFieldBuilder &SetFieldId(int32_t value);
	string TryBuild(optional<PartitionField> &result);
	PartitionField Build();

private:
	optional<int32_t> source_id_;
	optional<Transform> transform_;
	optional<string> name_;
	optional<int32_t> field_id_;
	bool has_source_id_ = false;
	bool has_transform_ = false;
	bool has_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
