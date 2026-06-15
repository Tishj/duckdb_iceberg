
#include "rest_catalog/objects/table_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TableUpdate::TableUpdate() {
}

TableUpdateBuilder::TableUpdateBuilder() {
}

TableUpdateBuilder &TableUpdateBuilder::SetAssignUuidupdate(AssignUUIDUpdate value) {
	result_.assign_uuidupdate = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value) {
	result_.upgrade_format_version_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSchemaUpdate(AddSchemaUpdate value) {
	result_.add_schema_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetCurrentSchemaUpdate(SetCurrentSchemaUpdate value) {
	result_.set_current_schema_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddPartitionSpecUpdate(AddPartitionSpecUpdate value) {
	result_.add_partition_spec_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetDefaultSpecUpdate(SetDefaultSpecUpdate value) {
	result_.set_default_spec_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSortOrderUpdate(AddSortOrderUpdate value) {
	result_.add_sort_order_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetDefaultSortOrderUpdate(SetDefaultSortOrderUpdate value) {
	result_.set_default_sort_order_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddSnapshotUpdate(AddSnapshotUpdate value) {
	result_.add_snapshot_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetSnapshotRefUpdate(SetSnapshotRefUpdate value) {
	result_.set_snapshot_ref_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSnapshotsUpdate(RemoveSnapshotsUpdate value) {
	result_.remove_snapshots_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSnapshotRefUpdate(RemoveSnapshotRefUpdate value) {
	result_.remove_snapshot_ref_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetLocationUpdate(SetLocationUpdate value) {
	result_.set_location_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetPropertiesUpdate(SetPropertiesUpdate value) {
	result_.set_properties_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemovePropertiesUpdate(RemovePropertiesUpdate value) {
	result_.remove_properties_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetSetStatisticsUpdate(SetStatisticsUpdate value) {
	result_.set_statistics_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveStatisticsUpdate(RemoveStatisticsUpdate value) {
	result_.remove_statistics_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate value) {
	result_.remove_partition_specs_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveSchemasUpdate(RemoveSchemasUpdate value) {
	result_.remove_schemas_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetAddEncryptionKeyUpdate(AddEncryptionKeyUpdate value) {
	result_.add_encryption_key_update = std::move(value);
	return *this;
}

TableUpdateBuilder &TableUpdateBuilder::SetRemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate value) {
	result_.remove_encryption_key_update = std::move(value);
	return *this;
}

string TableUpdateBuilder::TryBuild(TableUpdate &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

TableUpdate TableUpdateBuilder::Build() {
	TableUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

TableUpdate TableUpdate::FromJSON(yyjson_val *obj) {
	TableUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

TableUpdate TableUpdate::Copy() const {
	TableUpdate res;
	if (assign_uuidupdate.has_value()) {
		res.assign_uuidupdate.emplace();
		(*res.assign_uuidupdate) = (*assign_uuidupdate).Copy();
	}
	if (upgrade_format_version_update.has_value()) {
		res.upgrade_format_version_update.emplace();
		(*res.upgrade_format_version_update) = (*upgrade_format_version_update).Copy();
	}
	if (add_schema_update.has_value()) {
		res.add_schema_update.emplace();
		(*res.add_schema_update) = (*add_schema_update).Copy();
	}
	if (set_current_schema_update.has_value()) {
		res.set_current_schema_update.emplace();
		(*res.set_current_schema_update) = (*set_current_schema_update).Copy();
	}
	if (add_partition_spec_update.has_value()) {
		res.add_partition_spec_update.emplace();
		(*res.add_partition_spec_update) = (*add_partition_spec_update).Copy();
	}
	if (set_default_spec_update.has_value()) {
		res.set_default_spec_update.emplace();
		(*res.set_default_spec_update) = (*set_default_spec_update).Copy();
	}
	if (add_sort_order_update.has_value()) {
		res.add_sort_order_update.emplace();
		(*res.add_sort_order_update) = (*add_sort_order_update).Copy();
	}
	if (set_default_sort_order_update.has_value()) {
		res.set_default_sort_order_update.emplace();
		(*res.set_default_sort_order_update) = (*set_default_sort_order_update).Copy();
	}
	if (add_snapshot_update.has_value()) {
		res.add_snapshot_update.emplace();
		(*res.add_snapshot_update) = (*add_snapshot_update).Copy();
	}
	if (set_snapshot_ref_update.has_value()) {
		res.set_snapshot_ref_update.emplace();
		(*res.set_snapshot_ref_update) = (*set_snapshot_ref_update).Copy();
	}
	if (remove_snapshots_update.has_value()) {
		res.remove_snapshots_update.emplace();
		(*res.remove_snapshots_update) = (*remove_snapshots_update).Copy();
	}
	if (remove_snapshot_ref_update.has_value()) {
		res.remove_snapshot_ref_update.emplace();
		(*res.remove_snapshot_ref_update) = (*remove_snapshot_ref_update).Copy();
	}
	if (set_location_update.has_value()) {
		res.set_location_update.emplace();
		(*res.set_location_update) = (*set_location_update).Copy();
	}
	if (set_properties_update.has_value()) {
		res.set_properties_update.emplace();
		(*res.set_properties_update) = (*set_properties_update).Copy();
	}
	if (remove_properties_update.has_value()) {
		res.remove_properties_update.emplace();
		(*res.remove_properties_update) = (*remove_properties_update).Copy();
	}
	if (set_statistics_update.has_value()) {
		res.set_statistics_update.emplace();
		(*res.set_statistics_update) = (*set_statistics_update).Copy();
	}
	if (remove_statistics_update.has_value()) {
		res.remove_statistics_update.emplace();
		(*res.remove_statistics_update) = (*remove_statistics_update).Copy();
	}
	if (remove_partition_specs_update.has_value()) {
		res.remove_partition_specs_update.emplace();
		(*res.remove_partition_specs_update) = (*remove_partition_specs_update).Copy();
	}
	if (remove_schemas_update.has_value()) {
		res.remove_schemas_update.emplace();
		(*res.remove_schemas_update) = (*remove_schemas_update).Copy();
	}
	if (add_encryption_key_update.has_value()) {
		res.add_encryption_key_update.emplace();
		(*res.add_encryption_key_update) = (*add_encryption_key_update).Copy();
	}
	if (remove_encryption_key_update.has_value()) {
		res.remove_encryption_key_update.emplace();
		(*res.remove_encryption_key_update) = (*remove_encryption_key_update).Copy();
	}
	return res;
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

string TableUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	assign_uuidupdate.emplace();
	error = assign_uuidupdate->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		assign_uuidupdate = nullopt;
	}
	upgrade_format_version_update.emplace();
	error = upgrade_format_version_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		upgrade_format_version_update = nullopt;
	}
	add_schema_update.emplace();
	error = add_schema_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_schema_update = nullopt;
	}
	set_current_schema_update.emplace();
	error = set_current_schema_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_current_schema_update = nullopt;
	}
	add_partition_spec_update.emplace();
	error = add_partition_spec_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_partition_spec_update = nullopt;
	}
	set_default_spec_update.emplace();
	error = set_default_spec_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_default_spec_update = nullopt;
	}
	add_sort_order_update.emplace();
	error = add_sort_order_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_sort_order_update = nullopt;
	}
	set_default_sort_order_update.emplace();
	error = set_default_sort_order_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_default_sort_order_update = nullopt;
	}
	add_snapshot_update.emplace();
	error = add_snapshot_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_snapshot_update = nullopt;
	}
	set_snapshot_ref_update.emplace();
	error = set_snapshot_ref_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_snapshot_ref_update = nullopt;
	}
	remove_snapshots_update.emplace();
	error = remove_snapshots_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_snapshots_update = nullopt;
	}
	remove_snapshot_ref_update.emplace();
	error = remove_snapshot_ref_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_snapshot_ref_update = nullopt;
	}
	set_location_update.emplace();
	error = set_location_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_location_update = nullopt;
	}
	set_properties_update.emplace();
	error = set_properties_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_properties_update = nullopt;
	}
	remove_properties_update.emplace();
	error = remove_properties_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_properties_update = nullopt;
	}
	set_statistics_update.emplace();
	error = set_statistics_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_statistics_update = nullopt;
	}
	remove_statistics_update.emplace();
	error = remove_statistics_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_statistics_update = nullopt;
	}
	remove_partition_specs_update.emplace();
	error = remove_partition_specs_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_partition_specs_update = nullopt;
	}
	remove_schemas_update.emplace();
	error = remove_schemas_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_schemas_update = nullopt;
	}
	add_encryption_key_update.emplace();
	error = add_encryption_key_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_encryption_key_update = nullopt;
	}
	remove_encryption_key_update.emplace();
	error = remove_encryption_key_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_encryption_key_update = nullopt;
	}
	if (!(add_encryption_key_update.has_value()) && !(add_partition_spec_update.has_value()) &&
	    !(add_schema_update.has_value()) && !(add_snapshot_update.has_value()) &&
	    !(add_sort_order_update.has_value()) && !(assign_uuidupdate.has_value()) &&
	    !(remove_encryption_key_update.has_value()) && !(remove_partition_specs_update.has_value()) &&
	    !(remove_properties_update.has_value()) && !(remove_schemas_update.has_value()) &&
	    !(remove_snapshot_ref_update.has_value()) && !(remove_snapshots_update.has_value()) &&
	    !(remove_statistics_update.has_value()) && !(set_current_schema_update.has_value()) &&
	    !(set_default_sort_order_update.has_value()) && !(set_default_spec_update.has_value()) &&
	    !(set_location_update.has_value()) && !(set_properties_update.has_value()) &&
	    !(set_snapshot_ref_update.has_value()) && !(set_statistics_update.has_value()) &&
	    !(upgrade_format_version_update.has_value())) {
		return "TableUpdate failed to parse, none of the anyOf candidates matched";
	}
	return Validate();
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
