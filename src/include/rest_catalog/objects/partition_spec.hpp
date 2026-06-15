
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/partition_field.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PartitionSpecBuilder;

class PartitionSpec {
public:
	PartitionSpec(const PartitionSpec &) = delete;
	PartitionSpec &operator=(const PartitionSpec &) = delete;
	PartitionSpec(PartitionSpec &&) = default;
	PartitionSpec &operator=(PartitionSpec &&) = delete;

private:
	friend class PartitionSpecBuilder;
	PartitionSpec(vector<PartitionField> fields_p, optional<int32_t> spec_id_p);

public:
	// Deserialization
	static PartitionSpec FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<PartitionSpec> &result);
	string Validate() const;

	// Copy
	PartitionSpec Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<PartitionField> fields;
	optional<int32_t> spec_id;
};

class PartitionSpecBuilder {
public:
	PartitionSpecBuilder();
	PartitionSpecBuilder &SetFields(vector<PartitionField> value);
	PartitionSpecBuilder &SetSpecId(int32_t value);
	string TryBuild(optional<PartitionSpec> &result);
	PartitionSpec Build();

private:
	optional<vector<PartitionField>> fields_;
	optional<int32_t> spec_id_;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
