
#include "rest_catalog/objects/table_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TableUpdate::TableUpdate(
    optional<AssignUUIDUpdate> assign_uuidupdate_p,
    optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_p, optional<AddSchemaUpdate> add_schema_update_p,
    optional<SetCurrentSchemaUpdate> set_current_schema_update_p,
    optional<AddPartitionSpecUpdate> add_partition_spec_update_p,
    optional<SetDefaultSpecUpdate> set_default_spec_update_p, optional<AddSortOrderUpdate> add_sort_order_update_p,
    optional<SetDefaultSortOrderUpdate> set_default_sort_order_update_p,
    optional<AddSnapshotUpdate> add_snapshot_update_p, optional<SetSnapshotRefUpdate> set_snapshot_ref_update_p,
    optional<RemoveSnapshotsUpdate> remove_snapshots_update_p,
    optional<RemoveSnapshotRefUpdate> remove_snapshot_ref_update_p, optional<SetLocationUpdate> set_location_update_p,
    optional<SetPropertiesUpdate> set_properties_update_p, optional<RemovePropertiesUpdate> remove_properties_update_p,
    optional<SetStatisticsUpdate> set_statistics_update_p, optional<RemoveStatisticsUpdate> remove_statistics_update_p,
    optional<RemovePartitionSpecsUpdate> remove_partition_specs_update_p,
    optional<RemoveSchemasUpdate> remove_schemas_update_p, optional<AddEncryptionKeyUpdate> add_encryption_key_update_p,
    optional<RemoveEncryptionKeyUpdate> remove_encryption_key_update_p)
    : assign_uuidupdate(std::move(assign_uuidupdate_p)),
      upgrade_format_version_update(std::move(upgrade_format_version_update_p)),
      add_schema_update(std::move(add_schema_update_p)),
      set_current_schema_update(std::move(set_current_schema_update_p)),
      add_partition_spec_update(std::move(add_partition_spec_update_p)),
      set_default_spec_update(std::move(set_default_spec_update_p)),
      add_sort_order_update(std::move(add_sort_order_update_p)),
      set_default_sort_order_update(std::move(set_default_sort_order_update_p)),
      add_snapshot_update(std::move(add_snapshot_update_p)),
      set_snapshot_ref_update(std::move(set_snapshot_ref_update_p)),
      remove_snapshots_update(std::move(remove_snapshots_update_p)),
      remove_snapshot_ref_update(std::move(remove_snapshot_ref_update_p)),
      set_location_update(std::move(set_location_update_p)), set_properties_update(std::move(set_properties_update_p)),
      remove_properties_update(std::move(remove_properties_update_p)),
      set_statistics_update(std::move(set_statistics_update_p)),
      remove_statistics_update(std::move(remove_statistics_update_p)),
      remove_partition_specs_update(std::move(remove_partition_specs_update_p)),
      remove_schemas_update(std::move(remove_schemas_update_p)),
      add_encryption_key_update(std::move(add_encryption_key_update_p)),
      remove_encryption_key_update(std::move(remove_encryption_key_update_p)) {
}
TableUpdate::TableUpdate(const TableUpdate &other)
    : assign_uuidupdate((other.assign_uuidupdate.has_value()
                             ? optional<AssignUUIDUpdate>((*other.assign_uuidupdate).Copy())
                             : optional<AssignUUIDUpdate>())),
      upgrade_format_version_update(
          (other.upgrade_format_version_update.has_value()
               ? optional<UpgradeFormatVersionUpdate>((*other.upgrade_format_version_update).Copy())
               : optional<UpgradeFormatVersionUpdate>())),
      add_schema_update((other.add_schema_update.has_value()
                             ? optional<AddSchemaUpdate>((*other.add_schema_update).Copy())
                             : optional<AddSchemaUpdate>())),
      set_current_schema_update((other.set_current_schema_update.has_value()
                                     ? optional<SetCurrentSchemaUpdate>((*other.set_current_schema_update).Copy())
                                     : optional<SetCurrentSchemaUpdate>())),
      add_partition_spec_update((other.add_partition_spec_update.has_value()
                                     ? optional<AddPartitionSpecUpdate>((*other.add_partition_spec_update).Copy())
                                     : optional<AddPartitionSpecUpdate>())),
      set_default_spec_update((other.set_default_spec_update.has_value()
                                   ? optional<SetDefaultSpecUpdate>((*other.set_default_spec_update).Copy())
                                   : optional<SetDefaultSpecUpdate>())),
      add_sort_order_update((other.add_sort_order_update.has_value()
                                 ? optional<AddSortOrderUpdate>((*other.add_sort_order_update).Copy())
                                 : optional<AddSortOrderUpdate>())),
      set_default_sort_order_update(
          (other.set_default_sort_order_update.has_value()
               ? optional<SetDefaultSortOrderUpdate>((*other.set_default_sort_order_update).Copy())
               : optional<SetDefaultSortOrderUpdate>())),
      add_snapshot_update((other.add_snapshot_update.has_value()
                               ? optional<AddSnapshotUpdate>((*other.add_snapshot_update).Copy())
                               : optional<AddSnapshotUpdate>())),
      set_snapshot_ref_update((other.set_snapshot_ref_update.has_value()
                                   ? optional<SetSnapshotRefUpdate>((*other.set_snapshot_ref_update).Copy())
                                   : optional<SetSnapshotRefUpdate>())),
      remove_snapshots_update((other.remove_snapshots_update.has_value()
                                   ? optional<RemoveSnapshotsUpdate>((*other.remove_snapshots_update).Copy())
                                   : optional<RemoveSnapshotsUpdate>())),
      remove_snapshot_ref_update((other.remove_snapshot_ref_update.has_value()
                                      ? optional<RemoveSnapshotRefUpdate>((*other.remove_snapshot_ref_update).Copy())
                                      : optional<RemoveSnapshotRefUpdate>())),
      set_location_update((other.set_location_update.has_value()
                               ? optional<SetLocationUpdate>((*other.set_location_update).Copy())
                               : optional<SetLocationUpdate>())),
      set_properties_update((other.set_properties_update.has_value()
                                 ? optional<SetPropertiesUpdate>((*other.set_properties_update).Copy())
                                 : optional<SetPropertiesUpdate>())),
      remove_properties_update((other.remove_properties_update.has_value()
                                    ? optional<RemovePropertiesUpdate>((*other.remove_properties_update).Copy())
                                    : optional<RemovePropertiesUpdate>())),
      set_statistics_update((other.set_statistics_update.has_value()
                                 ? optional<SetStatisticsUpdate>((*other.set_statistics_update).Copy())
                                 : optional<SetStatisticsUpdate>())),
      remove_statistics_update((other.remove_statistics_update.has_value()
                                    ? optional<RemoveStatisticsUpdate>((*other.remove_statistics_update).Copy())
                                    : optional<RemoveStatisticsUpdate>())),
      remove_partition_specs_update(
          (other.remove_partition_specs_update.has_value()
               ? optional<RemovePartitionSpecsUpdate>((*other.remove_partition_specs_update).Copy())
               : optional<RemovePartitionSpecsUpdate>())),
      remove_schemas_update((other.remove_schemas_update.has_value()
                                 ? optional<RemoveSchemasUpdate>((*other.remove_schemas_update).Copy())
                                 : optional<RemoveSchemasUpdate>())),
      add_encryption_key_update((other.add_encryption_key_update.has_value()
                                     ? optional<AddEncryptionKeyUpdate>((*other.add_encryption_key_update).Copy())
                                     : optional<AddEncryptionKeyUpdate>())),
      remove_encryption_key_update(
          (other.remove_encryption_key_update.has_value()
               ? optional<RemoveEncryptionKeyUpdate>((*other.remove_encryption_key_update).Copy())
               : optional<RemoveEncryptionKeyUpdate>())) {
}
TableUpdate::TableUpdate(TableUpdate &&other) : TableUpdate(static_cast<const TableUpdate &>(other)) {
}

TableUpdateBuilder::TableUpdateBuilder() {
}

TableUpdateBuilder &TableUpdateBuilder::SetAssignUuidupdate(AssignUUIDUpdate value) {
	assign_uuidupdate_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value) {
	upgrade_format_version_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSchemaUpdate(AddSchemaUpdate value) {
	add_schema_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetCurrentSchemaUpdate(SetCurrentSchemaUpdate value) {
	set_current_schema_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddPartitionSpecUpdate(AddPartitionSpecUpdate value) {
	add_partition_spec_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetDefaultSpecUpdate(SetDefaultSpecUpdate value) {
	set_default_spec_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSortOrderUpdate(AddSortOrderUpdate value) {
	add_sort_order_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate value) {
	set_default_sort_order_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSnapshotUpdate(AddSnapshotUpdate value) {
	add_snapshot_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetSnapshotRefUpdate(SetSnapshotRefUpdate value) {
	set_snapshot_ref_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSnapshotsUpdate(RemoveSnapshotsUpdate value) {
	remove_snapshots_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate value) {
	remove_snapshot_ref_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetLocationUpdate(SetLocationUpdate value) {
	set_location_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetPropertiesUpdate(SetPropertiesUpdate value) {
	set_properties_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemovePropertiesUpdate(RemovePropertiesUpdate value) {
	remove_properties_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetStatisticsUpdate(SetStatisticsUpdate value) {
	set_statistics_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveStatisticsUpdate(RemoveStatisticsUpdate value) {
	remove_statistics_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate value) {
	remove_partition_specs_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSchemasUpdate(RemoveSchemasUpdate value) {
	remove_schemas_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddEncryptionKeyUpdate(AddEncryptionKeyUpdate value) {
	add_encryption_key_update_.emplace(std::move(value));
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate value) {
	remove_encryption_key_update_.emplace(std::move(value));
	return *this;
}

TableUpdate TableUpdateBuilder::Build() {
	auto result = TableUpdate(
	    std::move(assign_uuidupdate_), std::move(upgrade_format_version_update_), std::move(add_schema_update_),
	    std::move(set_current_schema_update_), std::move(add_partition_spec_update_),
	    std::move(set_default_spec_update_), std::move(add_sort_order_update_),
	    std::move(set_default_sort_order_update_), std::move(add_snapshot_update_), std::move(set_snapshot_ref_update_),
	    std::move(remove_snapshots_update_), std::move(remove_snapshot_ref_update_), std::move(set_location_update_),
	    std::move(set_properties_update_), std::move(remove_properties_update_), std::move(set_statistics_update_),
	    std::move(remove_statistics_update_), std::move(remove_partition_specs_update_),
	    std::move(remove_schemas_update_), std::move(add_encryption_key_update_),
	    std::move(remove_encryption_key_update_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TableUpdateBuilder::TryBuild(optional<TableUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string TableUpdate::TryFromJSON(yyjson_val *obj, TableUpdateBuilder &builder) {
	try {
		int matched_any_of_variants = 0;
		try {
			builder.SetAssignUuidupdate(AssignUUIDUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetAddSchemaUpdate(AddSchemaUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetCurrentSchemaUpdate(SetCurrentSchemaUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetAddPartitionSpecUpdate(AddPartitionSpecUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetDefaultSpecUpdate(SetDefaultSpecUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetAddSortOrderUpdate(AddSortOrderUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetAddSnapshotUpdate(AddSnapshotUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetSnapshotRefUpdate(SetSnapshotRefUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemoveSnapshotsUpdate(RemoveSnapshotsUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetLocationUpdate(SetLocationUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetPropertiesUpdate(SetPropertiesUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemovePropertiesUpdate(RemovePropertiesUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetSetStatisticsUpdate(SetStatisticsUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemoveStatisticsUpdate(RemoveStatisticsUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemoveSchemasUpdate(RemoveSchemasUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetAddEncryptionKeyUpdate(AddEncryptionKeyUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetRemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		if (matched_any_of_variants == 0) {
			throw InvalidInputException("TableUpdate failed to parse, none of the anyOf candidates matched");
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableUpdate TableUpdate::FromJSON(yyjson_val *obj) {
	TableUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

TableUpdate TableUpdate::Copy() const {
	return TableUpdate(*this);
}

string TableUpdate::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (assign_uuidupdate.has_value()) {
		matched_any_of_variants++;
		error = assign_uuidupdate->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (upgrade_format_version_update.has_value()) {
		matched_any_of_variants++;
		error = upgrade_format_version_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_schema_update.has_value()) {
		matched_any_of_variants++;
		error = add_schema_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_current_schema_update.has_value()) {
		matched_any_of_variants++;
		error = set_current_schema_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_partition_spec_update.has_value()) {
		matched_any_of_variants++;
		error = add_partition_spec_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_default_spec_update.has_value()) {
		matched_any_of_variants++;
		error = set_default_spec_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_sort_order_update.has_value()) {
		matched_any_of_variants++;
		error = add_sort_order_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_default_sort_order_update.has_value()) {
		matched_any_of_variants++;
		error = set_default_sort_order_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_snapshot_update.has_value()) {
		matched_any_of_variants++;
		error = add_snapshot_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_snapshot_ref_update.has_value()) {
		matched_any_of_variants++;
		error = set_snapshot_ref_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_snapshots_update.has_value()) {
		matched_any_of_variants++;
		error = remove_snapshots_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_snapshot_ref_update.has_value()) {
		matched_any_of_variants++;
		error = remove_snapshot_ref_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_location_update.has_value()) {
		matched_any_of_variants++;
		error = set_location_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_properties_update.has_value()) {
		matched_any_of_variants++;
		error = set_properties_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_properties_update.has_value()) {
		matched_any_of_variants++;
		error = remove_properties_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_statistics_update.has_value()) {
		matched_any_of_variants++;
		error = set_statistics_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_statistics_update.has_value()) {
		matched_any_of_variants++;
		error = remove_statistics_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_partition_specs_update.has_value()) {
		matched_any_of_variants++;
		error = remove_partition_specs_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_schemas_update.has_value()) {
		matched_any_of_variants++;
		error = remove_schemas_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_encryption_key_update.has_value()) {
		matched_any_of_variants++;
		error = add_encryption_key_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_encryption_key_update.has_value()) {
		matched_any_of_variants++;
		error = remove_encryption_key_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "TableUpdate must have at least one anyOf variant set";
	}
	return "";
}

void TableUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (assign_uuidupdate.has_value()) {
		assign_uuidupdate->PopulateJSON(doc, obj);
	} else if (upgrade_format_version_update.has_value()) {
		upgrade_format_version_update->PopulateJSON(doc, obj);
	} else if (add_schema_update.has_value()) {
		add_schema_update->PopulateJSON(doc, obj);
	} else if (set_current_schema_update.has_value()) {
		set_current_schema_update->PopulateJSON(doc, obj);
	} else if (add_partition_spec_update.has_value()) {
		add_partition_spec_update->PopulateJSON(doc, obj);
	} else if (set_default_spec_update.has_value()) {
		set_default_spec_update->PopulateJSON(doc, obj);
	} else if (add_sort_order_update.has_value()) {
		add_sort_order_update->PopulateJSON(doc, obj);
	} else if (set_default_sort_order_update.has_value()) {
		set_default_sort_order_update->PopulateJSON(doc, obj);
	} else if (add_snapshot_update.has_value()) {
		add_snapshot_update->PopulateJSON(doc, obj);
	} else if (set_snapshot_ref_update.has_value()) {
		set_snapshot_ref_update->PopulateJSON(doc, obj);
	} else if (remove_snapshots_update.has_value()) {
		remove_snapshots_update->PopulateJSON(doc, obj);
	} else if (remove_snapshot_ref_update.has_value()) {
		remove_snapshot_ref_update->PopulateJSON(doc, obj);
	} else if (set_location_update.has_value()) {
		set_location_update->PopulateJSON(doc, obj);
	} else if (set_properties_update.has_value()) {
		set_properties_update->PopulateJSON(doc, obj);
	} else if (remove_properties_update.has_value()) {
		remove_properties_update->PopulateJSON(doc, obj);
	} else if (set_statistics_update.has_value()) {
		set_statistics_update->PopulateJSON(doc, obj);
	} else if (remove_statistics_update.has_value()) {
		remove_statistics_update->PopulateJSON(doc, obj);
	} else if (remove_partition_specs_update.has_value()) {
		remove_partition_specs_update->PopulateJSON(doc, obj);
	} else if (remove_schemas_update.has_value()) {
		remove_schemas_update->PopulateJSON(doc, obj);
	} else if (add_encryption_key_update.has_value()) {
		add_encryption_key_update->PopulateJSON(doc, obj);
	} else if (remove_encryption_key_update.has_value()) {
		remove_encryption_key_update->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *TableUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
