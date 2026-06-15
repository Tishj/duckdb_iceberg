
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/partition_spec.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddPartitionSpecUpdate {
public:
	AddPartitionSpecUpdate();
	AddPartitionSpecUpdate(const AddPartitionSpecUpdate &) = delete;
	AddPartitionSpecUpdate &operator=(const AddPartitionSpecUpdate &) = delete;
	AddPartitionSpecUpdate(AddPartitionSpecUpdate &&) = default;
	AddPartitionSpecUpdate &operator=(AddPartitionSpecUpdate &&) = default;

public:
	// Deserialization
	static AddPartitionSpecUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	AddPartitionSpecUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	PartitionSpec spec;
};

class AddPartitionSpecUpdateBuilder {
public:
	AddPartitionSpecUpdateBuilder();
	AddPartitionSpecUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddPartitionSpecUpdateBuilder &SetSpec(PartitionSpec value);
	string TryBuild(AddPartitionSpecUpdate &result);
	AddPartitionSpecUpdate Build();

private:
	AddPartitionSpecUpdate result_;
	bool has_spec_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
