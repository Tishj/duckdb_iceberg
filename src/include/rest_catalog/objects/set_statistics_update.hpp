
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/statistics_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetStatisticsUpdateBuilder;

class SetStatisticsUpdate {
public:
	SetStatisticsUpdate(const SetStatisticsUpdate &) = delete;
	SetStatisticsUpdate &operator=(const SetStatisticsUpdate &) = delete;
	SetStatisticsUpdate(SetStatisticsUpdate &&) = default;
	SetStatisticsUpdate &operator=(SetStatisticsUpdate &&) = default;

private:
	friend class SetStatisticsUpdateBuilder;
	friend class GeneratedObjectAccess;
	SetStatisticsUpdate();

public:
	// Deserialization
	static SetStatisticsUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetStatisticsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	StatisticsFile statistics;
	optional<int64_t> snapshot_id;
};

class SetStatisticsUpdateBuilder {
public:
	SetStatisticsUpdateBuilder();
	SetStatisticsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetStatisticsUpdateBuilder &SetStatistics(StatisticsFile value);
	SetStatisticsUpdateBuilder &SetSnapshotId(int64_t value);
	string TryBuild(SetStatisticsUpdate &result);
	SetStatisticsUpdate Build();

private:
	SetStatisticsUpdate result_;
	bool has_statistics_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
