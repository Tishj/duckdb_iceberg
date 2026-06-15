
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

class RemoveStatisticsUpdateBuilder;

class RemoveStatisticsUpdate {
public:
	RemoveStatisticsUpdate(const RemoveStatisticsUpdate &) = delete;
	RemoveStatisticsUpdate &operator=(const RemoveStatisticsUpdate &) = delete;
	RemoveStatisticsUpdate(RemoveStatisticsUpdate &&) = default;
	RemoveStatisticsUpdate &operator=(RemoveStatisticsUpdate &&) = delete;

private:
	friend class RemoveStatisticsUpdateBuilder;
	RemoveStatisticsUpdate(BaseUpdate base_update_p, int64_t snapshot_id_p);

public:
	// Deserialization
	static RemoveStatisticsUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<RemoveStatisticsUpdate> &result);
	string Validate() const;

	// Copy
	RemoveStatisticsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int64_t snapshot_id;
};

class RemoveStatisticsUpdateBuilder {
public:
	RemoveStatisticsUpdateBuilder();
	RemoveStatisticsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveStatisticsUpdateBuilder &SetSnapshotId(int64_t value);
	string TryBuild(optional<RemoveStatisticsUpdate> &result);
	RemoveStatisticsUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int64_t> snapshot_id_;
	bool has_snapshot_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
