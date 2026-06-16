
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/snapshot.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddSnapshotUpdateBuilder;

class AddSnapshotUpdate {
public:
	AddSnapshotUpdate(const AddSnapshotUpdate &) = delete;
	AddSnapshotUpdate &operator=(const AddSnapshotUpdate &) = delete;
	AddSnapshotUpdate(AddSnapshotUpdate &&) = default;
	AddSnapshotUpdate &operator=(AddSnapshotUpdate &&) = delete;

private:
	friend class AddSnapshotUpdateBuilder;
	AddSnapshotUpdate(BaseUpdate base_update_p, Snapshot snapshot_p);

public:
	// Deserialization
	static AddSnapshotUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AddSnapshotUpdateBuilder &builder);
	string Validate() const;

	// Copy
	AddSnapshotUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	Snapshot snapshot;
};

class AddSnapshotUpdateBuilder {
public:
	AddSnapshotUpdateBuilder();
	AddSnapshotUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddSnapshotUpdateBuilder &SetSnapshot(Snapshot value);
	string TryBuild(optional<AddSnapshotUpdate> &result);
	AddSnapshotUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<Snapshot> snapshot_;
	bool has_snapshot_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
