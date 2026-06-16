
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/partition_spec.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddPartitionSpecUpdateBuilder;

class AddPartitionSpecUpdate {
public:
	AddPartitionSpecUpdate(const AddPartitionSpecUpdate &);
	AddPartitionSpecUpdate &operator=(const AddPartitionSpecUpdate &) = delete;
	AddPartitionSpecUpdate(AddPartitionSpecUpdate &&);
	AddPartitionSpecUpdate &operator=(AddPartitionSpecUpdate &&) = delete;

private:
	friend class AddPartitionSpecUpdateBuilder;
	AddPartitionSpecUpdate(BaseUpdate base_update_p, PartitionSpec spec_p);

public:
	// Deserialization
	static AddPartitionSpecUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AddPartitionSpecUpdateBuilder &builder);
	string Validate() const;

	// Copy
	AddPartitionSpecUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const PartitionSpec spec;
};

class AddPartitionSpecUpdateBuilder {
public:
	AddPartitionSpecUpdateBuilder();
	AddPartitionSpecUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddPartitionSpecUpdateBuilder &SetSpec(PartitionSpec value);
	string TryBuild(optional<AddPartitionSpecUpdate> &result);
	AddPartitionSpecUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<PartitionSpec> spec_;
	bool has_spec_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
