
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/assert_create.hpp"
#include "rest_catalog/objects/assert_current_schema_id.hpp"
#include "rest_catalog/objects/assert_default_sort_order_id.hpp"
#include "rest_catalog/objects/assert_default_spec_id.hpp"
#include "rest_catalog/objects/assert_last_assigned_field_id.hpp"
#include "rest_catalog/objects/assert_last_assigned_partition_id.hpp"
#include "rest_catalog/objects/assert_ref_snapshot_id.hpp"
#include "rest_catalog/objects/assert_table_uuid.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TableRequirementBuilder;

class TableRequirement {
public:
	TableRequirement(const TableRequirement &);
	TableRequirement &operator=(const TableRequirement &) = delete;
	TableRequirement(TableRequirement &&);
	TableRequirement &operator=(TableRequirement &&) = delete;

private:
	friend class TableRequirementBuilder;
	TableRequirement(optional<AssertCreate> assert_create_p, optional<AssertTableUUID> assert_table_uuid_p,
	                 optional<AssertRefSnapshotId> assert_ref_snapshot_id_p,
	                 optional<AssertLastAssignedFieldId> assert_last_assigned_field_id_p,
	                 optional<AssertCurrentSchemaId> assert_current_schema_id_p,
	                 optional<AssertLastAssignedPartitionId> assert_last_assigned_partition_id_p,
	                 optional<AssertDefaultSpecId> assert_default_spec_id_p,
	                 optional<AssertDefaultSortOrderId> assert_default_sort_order_id_p);

public:
	// Deserialization
	static TableRequirement FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, TableRequirementBuilder &builder);
	string Validate() const;

	// Copy
	TableRequirement Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<AssertCreate> assert_create;
	const optional<AssertTableUUID> assert_table_uuid;
	const optional<AssertRefSnapshotId> assert_ref_snapshot_id;
	const optional<AssertLastAssignedFieldId> assert_last_assigned_field_id;
	const optional<AssertCurrentSchemaId> assert_current_schema_id;
	const optional<AssertLastAssignedPartitionId> assert_last_assigned_partition_id;
	const optional<AssertDefaultSpecId> assert_default_spec_id;
	const optional<AssertDefaultSortOrderId> assert_default_sort_order_id;
};

class TableRequirementBuilder {
public:
	TableRequirementBuilder();
	TableRequirementBuilder &SetAssertCreate(AssertCreate value);
	TableRequirementBuilder &SetAssertTableUuid(AssertTableUUID value);
	TableRequirementBuilder &SetAssertRefSnapshotId(AssertRefSnapshotId value);
	TableRequirementBuilder &SetAssertLastAssignedFieldId(AssertLastAssignedFieldId value);
	TableRequirementBuilder &SetAssertCurrentSchemaId(AssertCurrentSchemaId value);
	TableRequirementBuilder &SetAssertLastAssignedPartitionId(AssertLastAssignedPartitionId value);
	TableRequirementBuilder &SetAssertDefaultSpecId(AssertDefaultSpecId value);
	TableRequirementBuilder &SetAssertDefaultSortOrderId(AssertDefaultSortOrderId value);
	string TryBuild(optional<TableRequirement> &result);
	TableRequirement Build();

private:
	optional<AssertCreate> assert_create_;
	optional<AssertTableUUID> assert_table_uuid_;
	optional<AssertRefSnapshotId> assert_ref_snapshot_id_;
	optional<AssertLastAssignedFieldId> assert_last_assigned_field_id_;
	optional<AssertCurrentSchemaId> assert_current_schema_id_;
	optional<AssertLastAssignedPartitionId> assert_last_assigned_partition_id_;
	optional<AssertDefaultSpecId> assert_default_spec_id_;
	optional<AssertDefaultSortOrderId> assert_default_sort_order_id_;
};

} // namespace rest_api_objects
} // namespace duckdb
