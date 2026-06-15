
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
	RemoveSnapshotsUpdate(const RemoveSnapshotsUpdate &) = delete;
	RemoveSnapshotsUpdate &operator=(const RemoveSnapshotsUpdate &) = delete;
	RemoveSnapshotsUpdate(RemoveSnapshotsUpdate &&) = default;
	RemoveSnapshotsUpdate &operator=(RemoveSnapshotsUpdate &&) = default;

private:
	friend class RemoveSnapshotsUpdateBuilder;
	friend class GeneratedObjectAccess;
	RemoveSnapshotsUpdate();

public:
	// Deserialization
	static RemoveSnapshotsUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RemoveSnapshotsUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	vector<int64_t> snapshot_ids;
};

class RemoveSnapshotsUpdateBuilder {
public:
	RemoveSnapshotsUpdateBuilder();
	RemoveSnapshotsUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveSnapshotsUpdateBuilder &SetSnapshotIds(vector<int64_t> value);
	string TryBuild(RemoveSnapshotsUpdate &result);
	RemoveSnapshotsUpdate Build();

private:
	RemoveSnapshotsUpdate result_;
	bool has_snapshot_ids_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
