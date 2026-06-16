
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

class RemoveSnapshotsUpdateBuilder;

class RemoveSnapshotsUpdate {
public:
	RemoveSnapshotsUpdate(const RemoveSnapshotsUpdate &);
	RemoveSnapshotsUpdate &operator=(const RemoveSnapshotsUpdate &) = delete;
	RemoveSnapshotsUpdate(RemoveSnapshotsUpdate &&);
	RemoveSnapshotsUpdate &operator=(RemoveSnapshotsUpdate &&) = delete;

private:
	friend class RemoveSnapshotsUpdateBuilder;
	RemoveSnapshotsUpdate(BaseUpdate base_update_p, vector<int64_t> snapshot_ids_p);

public:
	// Deserialization
	static RemoveSnapshotsUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, RemoveSnapshotsUpdateBuilder &builder);
	string Validate() const;

	// Copy
	RemoveSnapshotsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const vector<int64_t> snapshot_ids;
};

class RemoveSnapshotsUpdateBuilder {
public:
	RemoveSnapshotsUpdateBuilder();
	RemoveSnapshotsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveSnapshotsUpdateBuilder &SetSnapshotIds(vector<int64_t> value);
	string TryBuild(optional<RemoveSnapshotsUpdate> &result);
	RemoveSnapshotsUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<vector<int64_t>> snapshot_ids_;
	bool has_snapshot_ids_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
