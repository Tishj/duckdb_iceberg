
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/add_encryption_key_update.hpp"
#include "rest_catalog/objects/add_partition_spec_update.hpp"
#include "rest_catalog/objects/add_schema_update.hpp"
#include "rest_catalog/objects/add_snapshot_update.hpp"
#include "rest_catalog/objects/add_sort_order_update.hpp"
#include "rest_catalog/objects/assign_uuidupdate.hpp"
#include "rest_catalog/objects/remove_encryption_key_update.hpp"
#include "rest_catalog/objects/remove_partition_specs_update.hpp"
#include "rest_catalog/objects/remove_properties_update.hpp"
#include "rest_catalog/objects/remove_schemas_update.hpp"
#include "rest_catalog/objects/remove_snapshot_ref_update.hpp"
#include "rest_catalog/objects/remove_snapshots_update.hpp"
#include "rest_catalog/objects/remove_statistics_update.hpp"
#include "rest_catalog/objects/set_current_schema_update.hpp"
#include "rest_catalog/objects/set_default_sort_order_update.hpp"
#include "rest_catalog/objects/set_default_spec_update.hpp"
#include "rest_catalog/objects/set_location_update.hpp"
#include "rest_catalog/objects/set_properties_update.hpp"
#include "rest_catalog/objects/set_snapshot_ref_update.hpp"
#include "rest_catalog/objects/set_statistics_update.hpp"
#include "rest_catalog/objects/upgrade_format_version_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TableUpdateBuilder;

class TableUpdate {
public:
	TableUpdate(const TableUpdate &) = delete;
	TableUpdate &operator=(const TableUpdate &) = delete;
	TableUpdate(TableUpdate &&) = default;
	TableUpdate &operator=(TableUpdate &&) = default;

private:
	friend class TableUpdateBuilder;
	friend class GeneratedObjectAccess;
	TableUpdate();

public:
	// Deserialization
	static TableUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	TableUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<AssignUUIDUpdate> assign_uuidupdate;
	optional<UpgradeFormatVersionUpdate> upgrade_format_version_update;
	optional<AddSchemaUpdate> add_schema_update;
	optional<SetCurrentSchemaUpdate> set_current_schema_update;
	optional<AddPartitionSpecUpdate> add_partition_spec_update;
	optional<SetDefaultSpecUpdate> set_default_spec_update;
	optional<AddSortOrderUpdate> add_sort_order_update;
	optional<SetDefaultSortOrderUpdate> set_default_sort_order_update;
	optional<AddSnapshotUpdate> add_snapshot_update;
	optional<SetSnapshotRefUpdate> set_snapshot_ref_update;
	optional<RemoveSnapshotsUpdate> remove_snapshots_update;
	optional<RemoveSnapshotRefUpdate> remove_snapshot_ref_update;
	optional<SetLocationUpdate> set_location_update;
	optional<SetPropertiesUpdate> set_properties_update;
	optional<RemovePropertiesUpdate> remove_properties_update;
	optional<SetStatisticsUpdate> set_statistics_update;
	optional<RemoveStatisticsUpdate> remove_statistics_update;
	optional<RemovePartitionSpecsUpdate> remove_partition_specs_update;
	optional<RemoveSchemasUpdate> remove_schemas_update;
	optional<AddEncryptionKeyUpdate> add_encryption_key_update;
	optional<RemoveEncryptionKeyUpdate> remove_encryption_key_update;
};

class TableUpdateBuilder {
public:
	TableUpdateBuilder();
	TableUpdateBuilder &SetAssignUuidupdate(AssignUUIDUpdate value);
	TableUpdateBuilder &SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value);
	TableUpdateBuilder &SetAddSchemaUpdate(AddSchemaUpdate value);
	TableUpdateBuilder &SetSetCurrentSchemaUpdate(SetCurrentSchemaUpdate value);
	TableUpdateBuilder &SetAddPartitionSpecUpdate(AddPartitionSpecUpdate value);
	TableUpdateBuilder &SetSetDefaultSpecUpdate(SetDefaultSpecUpdate value);
	TableUpdateBuilder &SetAddSortOrderUpdate(AddSortOrderUpdate value);
	TableUpdateBuilder &SetSetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate value);
	TableUpdateBuilder &SetAddSnapshotUpdate(AddSnapshotUpdate value);
	TableUpdateBuilder &SetSetSnapshotRefUpdate(SetSnapshotRefUpdate value);
	TableUpdateBuilder &SetRemoveSnapshotsUpdate(RemoveSnapshotsUpdate value);
	TableUpdateBuilder &SetRemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate value);
	TableUpdateBuilder &SetSetLocationUpdate(SetLocationUpdate value);
	TableUpdateBuilder &SetSetPropertiesUpdate(SetPropertiesUpdate value);
	TableUpdateBuilder &SetRemovePropertiesUpdate(RemovePropertiesUpdate value);
	TableUpdateBuilder &SetSetStatisticsUpdate(SetStatisticsUpdate value);
	TableUpdateBuilder &SetRemoveStatisticsUpdate(RemoveStatisticsUpdate value);
	TableUpdateBuilder &SetRemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate value);
	TableUpdateBuilder &SetRemoveSchemasUpdate(RemoveSchemasUpdate value);
	TableUpdateBuilder &SetAddEncryptionKeyUpdate(AddEncryptionKeyUpdate value);
	TableUpdateBuilder &SetRemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate value);
	string TryBuild(TableUpdate &result);
	TableUpdate Build();

private:
	TableUpdate result_;
};

} // namespace rest_api_objects
} // namespace duckdb
