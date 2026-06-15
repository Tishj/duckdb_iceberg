
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

class RemovePartitionStatisticsUpdateBuilder;

class RemovePartitionStatisticsUpdate {
public:
	RemovePartitionStatisticsUpdate(const RemovePartitionStatisticsUpdate &) = delete;
	RemovePartitionStatisticsUpdate &operator=(const RemovePartitionStatisticsUpdate &) = delete;
	RemovePartitionStatisticsUpdate(RemovePartitionStatisticsUpdate &&) = default;
	RemovePartitionStatisticsUpdate &operator=(RemovePartitionStatisticsUpdate &&) = default;

private:
	friend class RemovePartitionStatisticsUpdateBuilder;
	friend class GeneratedObjectAccess;
	RemovePartitionStatisticsUpdate();

public:
	// Deserialization
	static RemovePartitionStatisticsUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RemovePartitionStatisticsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int64_t snapshot_id;
};

class RemovePartitionStatisticsUpdateBuilder {
public:
	RemovePartitionStatisticsUpdateBuilder();
	RemovePartitionStatisticsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemovePartitionStatisticsUpdateBuilder &SetSnapshotId(int64_t value);
	string TryBuild(RemovePartitionStatisticsUpdate &result);
	RemovePartitionStatisticsUpdate Build();

private:
	RemovePartitionStatisticsUpdate result_;
	bool has_snapshot_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
