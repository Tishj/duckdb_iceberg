
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
	SetSnapshotRefUpdate(const SetSnapshotRefUpdate &);
	SetSnapshotRefUpdate &operator=(const SetSnapshotRefUpdate &) = delete;
	SetSnapshotRefUpdate(SetSnapshotRefUpdate &&);
	SetSnapshotRefUpdate &operator=(SetSnapshotRefUpdate &&) = delete;

private:
	friend class SetSnapshotRefUpdateBuilder;
	SetSnapshotRefUpdate(BaseUpdate base_update_p, SnapshotReference snapshot_reference_p, string ref_name_p);

public:
	// Deserialization
	static SetSnapshotRefUpdate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, SetSnapshotRefUpdateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	SetSnapshotRefUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const SnapshotReference snapshot_reference;
	const string ref_name;
};

class SetSnapshotRefUpdateBuilder {
public:
	SetSnapshotRefUpdateBuilder();
	SetSnapshotRefUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetSnapshotRefUpdateBuilder &SetSnapshotReference(SnapshotReference value);
	SetSnapshotRefUpdateBuilder &SetRefName(string value);
	optional<string> TryBuild(optional<SetSnapshotRefUpdate> &result);
	SetSnapshotRefUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<SnapshotReference> snapshot_reference_;
	optional<string> ref_name_;
	bool has_ref_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
