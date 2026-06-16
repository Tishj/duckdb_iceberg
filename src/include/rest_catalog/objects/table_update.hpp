
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
	TableUpdate(const TableUpdate &);
	TableUpdate &operator=(const TableUpdate &) = delete;
	TableUpdate(TableUpdate &&);
	TableUpdate &operator=(TableUpdate &&) = delete;

private:
	friend class TableUpdateBuilder;
	TableUpdate(
	    optional<AssignUUIDUpdate> assign_uuidupdate_p,
	    optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_p,
	    optional<AddSchemaUpdate> add_schema_update_p, optional<SetCurrentSchemaUpdate> set_current_schema_update_p,
	    optional<AddPartitionSpecUpdate> add_partition_spec_update_p,
	    optional<SetDefaultSpecUpdate> set_default_spec_update_p, optional<AddSortOrderUpdate> add_sort_order_update_p,
	    optional<SetDefaultSortOrderUpdate> set_default_sort_order_update_p,
	    optional<AddSnapshotUpdate> add_snapshot_update_p, optional<SetSnapshotRefUpdate> set_snapshot_ref_update_p,
	    optional<RemoveSnapshotsUpdate> remove_snapshots_update_p,
	    optional<RemoveSnapshotRefUpdate> remove_snapshot_ref_update_p,
	    optional<SetLocationUpdate> set_location_update_p, optional<SetPropertiesUpdate> set_properties_update_p,
	    optional<RemovePropertiesUpdate> remove_properties_update_p,
	    optional<SetStatisticsUpdate> set_statistics_update_p,
	    optional<RemoveStatisticsUpdate> remove_statistics_update_p,
	    optional<RemovePartitionSpecsUpdate> remove_partition_specs_update_p,
	    optional<RemoveSchemasUpdate> remove_schemas_update_p,
	    optional<AddEncryptionKeyUpdate> add_encryption_key_update_p,
	    optional<RemoveEncryptionKeyUpdate> remove_encryption_key_update_p);

public:
	// Deserialization
	static TableUpdate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, TableUpdateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	TableUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<AssignUUIDUpdate> assign_uuidupdate;
	const optional<UpgradeFormatVersionUpdate> upgrade_format_version_update;
	const optional<AddSchemaUpdate> add_schema_update;
	const optional<SetCurrentSchemaUpdate> set_current_schema_update;
	const optional<AddPartitionSpecUpdate> add_partition_spec_update;
	const optional<SetDefaultSpecUpdate> set_default_spec_update;
	const optional<AddSortOrderUpdate> add_sort_order_update;
	const optional<SetDefaultSortOrderUpdate> set_default_sort_order_update;
	const optional<AddSnapshotUpdate> add_snapshot_update;
	const optional<SetSnapshotRefUpdate> set_snapshot_ref_update;
	const optional<RemoveSnapshotsUpdate> remove_snapshots_update;
	const optional<RemoveSnapshotRefUpdate> remove_snapshot_ref_update;
	const optional<SetLocationUpdate> set_location_update;
	const optional<SetPropertiesUpdate> set_properties_update;
	const optional<RemovePropertiesUpdate> remove_properties_update;
	const optional<SetStatisticsUpdate> set_statistics_update;
	const optional<RemoveStatisticsUpdate> remove_statistics_update;
	const optional<RemovePartitionSpecsUpdate> remove_partition_specs_update;
	const optional<RemoveSchemasUpdate> remove_schemas_update;
	const optional<AddEncryptionKeyUpdate> add_encryption_key_update;
	const optional<RemoveEncryptionKeyUpdate> remove_encryption_key_update;
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
	optional<string> TryBuild(optional<TableUpdate> &result);
	TableUpdate Build();

private:
	optional<AssignUUIDUpdate> assign_uuidupdate_;
	optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_;
	optional<AddSchemaUpdate> add_schema_update_;
	optional<SetCurrentSchemaUpdate> set_current_schema_update_;
	optional<AddPartitionSpecUpdate> add_partition_spec_update_;
	optional<SetDefaultSpecUpdate> set_default_spec_update_;
	optional<AddSortOrderUpdate> add_sort_order_update_;
	optional<SetDefaultSortOrderUpdate> set_default_sort_order_update_;
	optional<AddSnapshotUpdate> add_snapshot_update_;
	optional<SetSnapshotRefUpdate> set_snapshot_ref_update_;
	optional<RemoveSnapshotsUpdate> remove_snapshots_update_;
	optional<RemoveSnapshotRefUpdate> remove_snapshot_ref_update_;
	optional<SetLocationUpdate> set_location_update_;
	optional<SetPropertiesUpdate> set_properties_update_;
	optional<RemovePropertiesUpdate> remove_properties_update_;
	optional<SetStatisticsUpdate> set_statistics_update_;
	optional<RemoveStatisticsUpdate> remove_statistics_update_;
	optional<RemovePartitionSpecsUpdate> remove_partition_specs_update_;
	optional<RemoveSchemasUpdate> remove_schemas_update_;
	optional<AddEncryptionKeyUpdate> add_encryption_key_update_;
	optional<RemoveEncryptionKeyUpdate> remove_encryption_key_update_;
};

} // namespace rest_api_objects
} // namespace duckdb
