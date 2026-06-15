
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

class RemoveSnapshotRefUpdateBuilder;

class RemoveSnapshotRefUpdate {
public:
	RemoveSnapshotRefUpdate(const RemoveSnapshotRefUpdate &) = delete;
	RemoveSnapshotRefUpdate &operator=(const RemoveSnapshotRefUpdate &) = delete;
	RemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate &&) = default;
	RemoveSnapshotRefUpdate &operator=(RemoveSnapshotRefUpdate &&) = default;

private:
	friend class RemoveSnapshotRefUpdateBuilder;
	friend class GeneratedObjectAccess;
	RemoveSnapshotRefUpdate();

public:
	// Deserialization
	static RemoveSnapshotRefUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RemoveSnapshotRefUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	string ref_name;
};

class RemoveSnapshotRefUpdateBuilder {
public:
	RemoveSnapshotRefUpdateBuilder();
	RemoveSnapshotRefUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveSnapshotRefUpdateBuilder &SetRefName(string value);
	string TryBuild(RemoveSnapshotRefUpdate &result);
	RemoveSnapshotRefUpdate Build();

private:
	RemoveSnapshotRefUpdate result_;
	bool has_ref_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
