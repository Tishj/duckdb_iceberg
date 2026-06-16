
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
	RemoveSnapshotRefUpdate(const RemoveSnapshotRefUpdate &);
	RemoveSnapshotRefUpdate &operator=(const RemoveSnapshotRefUpdate &) = delete;
	RemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate &&);
	RemoveSnapshotRefUpdate &operator=(RemoveSnapshotRefUpdate &&) = delete;

private:
	friend class RemoveSnapshotRefUpdateBuilder;
	RemoveSnapshotRefUpdate(BaseUpdate base_update_p, string ref_name_p);

public:
	// Deserialization
	static RemoveSnapshotRefUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, RemoveSnapshotRefUpdateBuilder &builder);
	string Validate() const;

	// Copy
	RemoveSnapshotRefUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const string ref_name;
};

class RemoveSnapshotRefUpdateBuilder {
public:
	RemoveSnapshotRefUpdateBuilder();
	RemoveSnapshotRefUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveSnapshotRefUpdateBuilder &SetRefName(string value);
	string TryBuild(optional<RemoveSnapshotRefUpdate> &result);
	RemoveSnapshotRefUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<string> ref_name_;
	bool has_ref_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
