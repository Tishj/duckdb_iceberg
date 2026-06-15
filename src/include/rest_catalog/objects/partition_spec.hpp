
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/partition_field.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PartitionSpec {
public:
	PartitionSpec();
	PartitionSpec(const PartitionSpec &) = delete;
	PartitionSpec &operator=(const PartitionSpec &) = delete;
	PartitionSpec(PartitionSpec &&) = default;
	PartitionSpec &operator=(PartitionSpec &&) = default;

public:
	// Deserialization
	static PartitionSpec FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(PartitionSpec &result);
	PartitionSpec Build();

private:
	PartitionSpec result_;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
