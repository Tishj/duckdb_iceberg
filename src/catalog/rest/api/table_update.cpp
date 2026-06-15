#include "catalog/rest/api/table_update.hpp"
#include "duckdb/common/exception.hpp"
#include "catalog/rest/iceberg_table_set.hpp"

namespace duckdb {

static rest_api_objects::TableRequirementType MakeRequirementType(const string &value) {
	rest_api_objects::TableRequirementType requirement_type;
	requirement_type.value = value;
	return requirement_type;
}

static rest_api_objects::BaseUpdate MakeBaseUpdate(const string &action) {
	return rest_api_objects::BaseUpdateBuilder().SetAction(action).Build();
}

static rest_api_objects::Schema CopySchema(const IcebergTableSchema &schema) {
	// the rest api objects are currently not copyable. Without having to modify generated code
	//  the easiest way to copy for now is to write the schema to string, then parse it again
	std::unique_ptr<yyjson_mut_doc, YyjsonDocDeleter> doc_p(yyjson_mut_doc_new(nullptr));
	yyjson_mut_doc *doc = doc_p.get();
	auto root_object = yyjson_mut_obj(doc);
	yyjson_mut_doc_set_root(doc, root_object);
	IcebergCreateTableRequest::PopulateSchema(doc, root_object, schema);
	auto schema_str = ICUtils::JsonToString(std::move(doc_p));

	// Parse it back as immutable
	std::unique_ptr<yyjson_doc, YyjsonDocDeleter> new_doc(
	    yyjson_read(schema_str.c_str(), strlen(schema_str.c_str()), 0));
	yyjson_val *val = yyjson_doc_get_root(new_doc.get());
	return rest_api_objects::Schema::FromJSON(val);
}

AddSchemaUpdate::AddSchemaUpdate(const IcebergTableInformation &table_info, int32_t schema_id)
    : IcebergTableUpdate(IcebergTableUpdateType::ADD_SCHEMA, table_info), schema_id(schema_id) {
	if (table_info.table_metadata.HasLastColumnId()) {
		last_column_id = table_info.table_metadata.GetLastColumnId();
	}
}

void AddSchemaUpdate::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                   IcebergCommitState &commit_state) const {
	auto &schemas = table_info.table_metadata.GetSchemas();
	auto it = schemas.find(schema_id);
	if (it == schemas.end()) {
		throw InternalException("(AddSchemaUpdate) Couldn't find schema with id: %d", schema_id);
	}
	auto &schema = it->second;
	auto update = rest_api_objects::AddSchemaUpdateBuilder()
	                  .SetBaseUpdate(MakeBaseUpdate("add-schema"))
	                  .SetSchema(CopySchema(*schema.get()))
	                  .Build();
	// last-column-id is technically deprecated in AddSchemaUpdate, but some catalogs still use it (nessie).
	if (last_column_id.IsValid()) {
		update.last_column_id = last_column_id.GetIndex();
	}
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetAddSchemaUpdate(std::move(update)).Build());
}

AssignUUIDUpdate::AssignUUIDUpdate(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::ADD_SCHEMA, table_info) {
}

void AssignUUIDUpdate::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                    IcebergCommitState &commit_state) const {
	auto update = rest_api_objects::AssignUUIDUpdateBuilder()
	                  .SetBaseUpdate(MakeBaseUpdate("assign-uuid"))
	                  .SetUuid(table_info.table_metadata.table_uuid)
	                  .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetAssignUuidupdate(std::move(update)).Build());
}

AssertCreateRequirement::AssertCreateRequirement(const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_CREATE, table_info) {
}

void AssertCreateRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertCreateBuilder().SetType(MakeRequirementType("assert-create")).Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertCreate(std::move(req)).Build());
}

AssertTableUUIDRequirement::AssertTableUUIDRequirement(const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_TABLE_UUID, table_info) {
}

void AssertTableUUIDRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                   IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertTableUUIDBuilder()
	               .SetType(MakeRequirementType("assert-table-uuid"))
	               .SetUuid(commit_state.table_info.table_metadata.table_uuid)
	               .Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertTableUuid(std::move(req)).Build());
}

AssertCurrentSchemaIdRequirement::AssertCurrentSchemaIdRequirement(const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_CURRENT_SCHEMA_ID, table_info) {
	current_schema_id = table_info.table_metadata.GetCurrentSchemaId();
}

void AssertCurrentSchemaIdRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                         IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertCurrentSchemaIdBuilder()
	               .SetType(MakeRequirementType("assert-current-schema-id"))
	               .SetCurrentSchemaId(current_schema_id)
	               .Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertCurrentSchemaId(std::move(req)).Build());
}

AssertLastAssignedFieldIdRequirement::AssertLastAssignedFieldIdRequirement(const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_LAST_ASSIGNED_FIELD_ID, table_info) {
	D_ASSERT(table_info.table_metadata.HasLastColumnId());
	last_assigned_field_id = static_cast<int32_t>(table_info.table_metadata.GetLastColumnId());
}

void AssertLastAssignedFieldIdRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                             IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertLastAssignedFieldIdBuilder()
	               .SetType(MakeRequirementType("assert-last-assigned-field-id"))
	               .SetLastAssignedFieldId(last_assigned_field_id)
	               .Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertLastAssignedFieldId(std::move(req)).Build());
}

AssertLastAssignedPartitionIdRequirement::AssertLastAssignedPartitionIdRequirement(
    const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_LAST_ASSIGNED_PARTITION_ID, table_info) {
	if (table_info.table_metadata.HasLastPartitionId()) {
		last_assigned_partition_id = table_info.table_metadata.GetLastPartitionFieldId();
	} else {
		// If no partition field IDs have been assigned, use 999 as the last assigned so 1000 becomes the
		// next partition id. Based on assignments in v1 in https://iceberg.apache.org/spec/#partition-evolution
		last_assigned_partition_id = 999;
	}
}

void AssertLastAssignedPartitionIdRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                                 IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertLastAssignedPartitionIdBuilder()
	               .SetType(MakeRequirementType("assert-last-assigned-partition-id"))
	               .SetLastAssignedPartitionId(last_assigned_partition_id)
	               .Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertLastAssignedPartitionId(std::move(req)).Build());
}

AssertDefaultSpecIdRequirement::AssertDefaultSpecIdRequirement(const IcebergTableInformation &table_info)
    : IcebergTableRequirement(IcebergTableRequirementType::ASSERT_DEFAULT_SPEC_ID, table_info) {
	default_spec_id = table_info.table_metadata.default_spec_id;
}

void AssertDefaultSpecIdRequirement::CreateRequirement(DatabaseInstance &db, ClientContext &context,
                                                       IcebergCommitState &commit_state) {
	auto req = rest_api_objects::AssertDefaultSpecIdBuilder()
	               .SetType(MakeRequirementType("assert-default-spec-id"))
	               .SetDefaultSpecId(default_spec_id)
	               .Build();
	commit_state.table_change.requirements.push_back(
	    rest_api_objects::TableRequirementBuilder().SetAssertDefaultSpecId(std::move(req)).Build());
}

UpgradeFormatVersion::UpgradeFormatVersion(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::UPGRADE_FORMAT_VERSION, table_info) {
}

void UpgradeFormatVersion::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                        IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::UpgradeFormatVersionUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("upgrade-format-version"))
	               .SetFormatVersion(table_info.table_metadata.iceberg_version)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetUpgradeFormatVersionUpdate(std::move(req)).Build());
}

SetCurrentSchema::SetCurrentSchema(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_CURRENT_SCHEMA, table_info) {
}

void SetCurrentSchema::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                    IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::SetCurrentSchemaUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("set-current-schema"))
	               .SetSchemaId(table_info.table_metadata.GetCurrentSchemaId())
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetSetCurrentSchemaUpdate(std::move(req)).Build());
}

AddPartitionSpec::AddPartitionSpec(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::ADD_PARTITION_SPEC, table_info) {
}

void AddPartitionSpec::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                    IcebergCommitState &commit_state) const {
	vector<rest_api_objects::PartitionField> fields;
	if (table_info.table_metadata.HasPartitionSpec()) {
		auto &current_partition_spec = table_info.table_metadata.GetLatestPartitionSpec();
		for (auto &field : current_partition_spec.fields) {
			rest_api_objects::Transform transform;
			transform.value = field.transform.RawType();
			auto updated_field = rest_api_objects::PartitionFieldBuilder()
			                         .SetSourceId(field.source_id)
			                         .SetTransform(std::move(transform))
			                         .SetName(field.GetPartitionSpecFieldName())
			                         .Build();
			updated_field.field_id = field.partition_field_id;
			fields.push_back(std::move(updated_field));
		}
	}
	auto spec = rest_api_objects::PartitionSpecBuilder().SetFields(std::move(fields)).Build();
	spec.spec_id = table_info.table_metadata.default_spec_id;
	auto req = rest_api_objects::AddPartitionSpecUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("add-spec"))
	               .SetSpec(std::move(spec))
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetAddPartitionSpecUpdate(std::move(req)).Build());
}

AddSortOrder::AddSortOrder(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::ADD_SORT_ORDER, table_info) {
}

void AddSortOrder::CreateUpdate(DatabaseInstance &db, ClientContext &context, IcebergCommitState &commit_state) const {
	vector<rest_api_objects::SortField> fields;
	int32_t order_id = 0;
	if (table_info.table_metadata.HasSortOrder()) {
		order_id = table_info.table_metadata.default_sort_order_id.GetIndex();
	}

	if (table_info.table_metadata.HasSortOrder()) {
		// FIXME: is it correct to just get the latest sort order?
		auto &current_sort_order = table_info.table_metadata.GetLatestSortOrder();
		for (auto &field : current_sort_order.fields) {
			rest_api_objects::SortDirection direction;
			direction.value = field.direction;
			rest_api_objects::Transform transform;
			transform.value = field.transform.RawType();
			rest_api_objects::NullOrder null_order;
			null_order.value = field.null_order;
			auto updated_field = rest_api_objects::SortFieldBuilder()
			                         .SetSourceId(field.source_id)
			                         .SetTransform(std::move(transform))
			                         .SetDirection(std::move(direction))
			                         .SetNullOrder(std::move(null_order))
			                         .Build();
			fields.push_back(std::move(updated_field));
		}
	}
	auto sort_order = rest_api_objects::SortOrderBuilder().SetOrderId(order_id).SetFields(std::move(fields)).Build();
	auto req = rest_api_objects::AddSortOrderUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("add-sort-order"))
	               .SetSortOrder(std::move(sort_order))
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetAddSortOrderUpdate(std::move(req)).Build());
}

SetDefaultSortOrder::SetDefaultSortOrder(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_DEFAULT_SORT_ORDER, table_info) {
}

void SetDefaultSortOrder::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                       IcebergCommitState &commit_state) const {
	D_ASSERT(table_info.table_metadata.HasSortOrder());
	auto req = rest_api_objects::SetDefaultSortOrderUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("set-default-sort-order"))
	               .SetSortOrderId(table_info.table_metadata.GetLatestSortOrder().sort_order_id)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetSetDefaultSortOrderUpdate(std::move(req)).Build());
}

SetDefaultSpec::SetDefaultSpec(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_DEFAULT_SPEC, table_info) {
}

void SetDefaultSpec::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                  IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::SetDefaultSpecUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("set-default-spec"))
	               .SetSpecId(table_info.table_metadata.default_spec_id)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetSetDefaultSpecUpdate(std::move(req)).Build());
}

SetProperties::SetProperties(const IcebergTableInformation &table_info,
                             const case_insensitive_map_t<string> &properties)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_PROPERTIES, table_info), properties(properties) {
}

void SetProperties::CreateUpdate(DatabaseInstance &db, ClientContext &context, IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::SetPropertiesUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("set-properties"))
	               .SetUpdates(properties)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetSetPropertiesUpdate(std::move(req)).Build());
}

RemoveProperties::RemoveProperties(const IcebergTableInformation &table_info, const vector<string> &properties)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_PROPERTIES, table_info), properties(properties) {
}

void RemoveProperties::CreateUpdate(DatabaseInstance &db, ClientContext &context,
                                    IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::RemovePropertiesUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("remove-properties"))
	               .SetRemovals(properties)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetRemovePropertiesUpdate(std::move(req)).Build());
}

SetLocation::SetLocation(const IcebergTableInformation &table_info)
    : IcebergTableUpdate(IcebergTableUpdateType::SET_LOCATION, table_info) {
}

void SetLocation::CreateUpdate(DatabaseInstance &db, ClientContext &context, IcebergCommitState &commit_state) const {
	auto req = rest_api_objects::SetLocationUpdateBuilder()
	               .SetBaseUpdate(MakeBaseUpdate("set-location"))
	               .SetLocation(table_info.table_metadata.location)
	               .Build();
	commit_state.table_change.updates.push_back(
	    rest_api_objects::TableUpdateBuilder().SetSetLocationUpdate(std::move(req)).Build());
}

} // namespace duckdb
