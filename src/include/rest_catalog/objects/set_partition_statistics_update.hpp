
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/partition_statistics_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetPartitionStatisticsUpdateBuilder;

class SetPartitionStatisticsUpdate {
public:
	SetPartitionStatisticsUpdate(const SetPartitionStatisticsUpdate &);
	SetPartitionStatisticsUpdate &operator=(const SetPartitionStatisticsUpdate &) = delete;
	SetPartitionStatisticsUpdate(SetPartitionStatisticsUpdate &&);
	SetPartitionStatisticsUpdate &operator=(SetPartitionStatisticsUpdate &&) = delete;

private:
	friend class SetPartitionStatisticsUpdateBuilder;
	SetPartitionStatisticsUpdate(BaseUpdate base_update_p, PartitionStatisticsFile partition_statistics_p);

public:
	// Deserialization
	static SetPartitionStatisticsUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SetPartitionStatisticsUpdateBuilder &builder);
	string Validate() const;

	// Copy
	SetPartitionStatisticsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const PartitionStatisticsFile partition_statistics;
};

class SetPartitionStatisticsUpdateBuilder {
public:
	SetPartitionStatisticsUpdateBuilder();
	SetPartitionStatisticsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetPartitionStatisticsUpdateBuilder &SetPartitionStatistics(PartitionStatisticsFile value);
	string TryBuild(optional<SetPartitionStatisticsUpdate> &result);
	SetPartitionStatisticsUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<PartitionStatisticsFile> partition_statistics_;
	bool has_partition_statistics_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
