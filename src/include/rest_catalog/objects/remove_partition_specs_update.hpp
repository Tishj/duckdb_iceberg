
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

class RemovePartitionSpecsUpdateBuilder;

class RemovePartitionSpecsUpdate {
public:
	RemovePartitionSpecsUpdate(const RemovePartitionSpecsUpdate &) = delete;
	RemovePartitionSpecsUpdate &operator=(const RemovePartitionSpecsUpdate &) = delete;
	RemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate &&) = default;
	RemovePartitionSpecsUpdate &operator=(RemovePartitionSpecsUpdate &&) = delete;

private:
	friend class RemovePartitionSpecsUpdateBuilder;
	RemovePartitionSpecsUpdate(BaseUpdate base_update_p, vector<int32_t> spec_ids_p);

public:
	// Deserialization
	static RemovePartitionSpecsUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, RemovePartitionSpecsUpdateBuilder &builder);
	string Validate() const;

	// Copy
	RemovePartitionSpecsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	vector<int32_t> spec_ids;
};

class RemovePartitionSpecsUpdateBuilder {
public:
	RemovePartitionSpecsUpdateBuilder();
	RemovePartitionSpecsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemovePartitionSpecsUpdateBuilder &SetSpecIds(vector<int32_t> value);
	string TryBuild(optional<RemovePartitionSpecsUpdate> &result);
	RemovePartitionSpecsUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<vector<int32_t>> spec_ids_;
	bool has_spec_ids_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
