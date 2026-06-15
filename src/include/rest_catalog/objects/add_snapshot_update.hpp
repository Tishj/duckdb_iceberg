
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/snapshot.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddSnapshotUpdate {
public:
	AddSnapshotUpdate();
	AddSnapshotUpdate(const AddSnapshotUpdate &) = delete;
	AddSnapshotUpdate &operator=(const AddSnapshotUpdate &) = delete;
	AddSnapshotUpdate(AddSnapshotUpdate &&) = default;
	AddSnapshotUpdate &operator=(AddSnapshotUpdate &&) = default;

public:
	// Deserialization
	static AddSnapshotUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(AddSnapshotUpdate &result);
	AddSnapshotUpdate Build();

private:
	AddSnapshotUpdate result_;
	bool has_snapshot_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
