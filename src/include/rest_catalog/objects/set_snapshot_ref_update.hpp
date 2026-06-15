
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/snapshot_reference.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetSnapshotRefUpdateBuilder;

class SetSnapshotRefUpdate {
public:
	SetSnapshotRefUpdate(const SetSnapshotRefUpdate &) = delete;
	SetSnapshotRefUpdate &operator=(const SetSnapshotRefUpdate &) = delete;
	SetSnapshotRefUpdate(SetSnapshotRefUpdate &&) = default;
	SetSnapshotRefUpdate &operator=(SetSnapshotRefUpdate &&) = default;

private:
	friend class SetSnapshotRefUpdateBuilder;
	friend class GeneratedObjectAccess;
	SetSnapshotRefUpdate();

public:
	// Deserialization
	static SetSnapshotRefUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetSnapshotRefUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	SnapshotReference snapshot_reference;
	string ref_name;
};

class SetSnapshotRefUpdateBuilder {
public:
	SetSnapshotRefUpdateBuilder();
	SetSnapshotRefUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetSnapshotRefUpdateBuilder &SetSnapshotReference(SnapshotReference value);
	SetSnapshotRefUpdateBuilder &SetRefName(string value);
	string TryBuild(SetSnapshotRefUpdate &result);
	SetSnapshotRefUpdate Build();

private:
	SetSnapshotRefUpdate result_;
	bool has_ref_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
