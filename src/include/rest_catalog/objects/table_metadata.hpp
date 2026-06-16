
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/encrypted_key.hpp"
#include "rest_catalog/objects/metadata_log.hpp"
#include "rest_catalog/objects/partition_spec.hpp"
#include "rest_catalog/objects/partition_statistics_file.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/snapshot.hpp"
#include "rest_catalog/objects/snapshot_log.hpp"
#include "rest_catalog/objects/snapshot_references.hpp"
#include "rest_catalog/objects/sort_order.hpp"
#include "rest_catalog/objects/statistics_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TableMetadataBuilder;

class TableMetadata {
public:
	TableMetadata(const TableMetadata &);
	TableMetadata &operator=(const TableMetadata &) = delete;
	TableMetadata(TableMetadata &&);
	TableMetadata &operator=(TableMetadata &&) = delete;

private:
	friend class TableMetadataBuilder;
	TableMetadata(int32_t format_version_p, string table_uuid_p, optional<string> location_p,
	              optional<int64_t> last_updated_ms_p, optional<int64_t> next_row_id_p,
	              optional<case_insensitive_map_t<string>> properties_p, optional<vector<Schema>> schemas_p,
	              optional<int32_t> current_schema_id_p, optional<int32_t> last_column_id_p,
	              optional<vector<PartitionSpec>> partition_specs_p, optional<int32_t> default_spec_id_p,
	              optional<int32_t> last_partition_id_p, optional<vector<SortOrder>> sort_orders_p,
	              optional<int32_t> default_sort_order_id_p, optional<vector<EncryptedKey>> encryption_keys_p,
	              optional<vector<Snapshot>> snapshots_p, optional<SnapshotReferences> refs_p,
	              optional<int64_t> current_snapshot_id_p, optional<int64_t> last_sequence_number_p,
	              optional<SnapshotLog> snapshot_log_p, optional<MetadataLog> metadata_log_p,
	              optional<vector<StatisticsFile>> statistics_p,
	              optional<vector<PartitionStatisticsFile>> partition_statistics_p);

public:
	// Deserialization
	static TableMetadata FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, TableMetadataBuilder &builder);
	optional<string> Validate() const;

	// Copy
	TableMetadata Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t format_version;
	const string table_uuid;
	const optional<string> location;
	const optional<int64_t> last_updated_ms;
	const optional<int64_t> next_row_id;
	const optional<case_insensitive_map_t<string>> properties;
	const optional<vector<Schema>> schemas;
	const optional<int32_t> current_schema_id;
	const optional<int32_t> last_column_id;
	const optional<vector<PartitionSpec>> partition_specs;
	const optional<int32_t> default_spec_id;
	const optional<int32_t> last_partition_id;
	const optional<vector<SortOrder>> sort_orders;
	const optional<int32_t> default_sort_order_id;
	const optional<vector<EncryptedKey>> encryption_keys;
	const optional<vector<Snapshot>> snapshots;
	const optional<SnapshotReferences> refs;
	const optional<int64_t> current_snapshot_id;
	const optional<int64_t> last_sequence_number;
	const optional<SnapshotLog> snapshot_log;
	const optional<MetadataLog> metadata_log;
	const optional<vector<StatisticsFile>> statistics;
	const optional<vector<PartitionStatisticsFile>> partition_statistics;
};

class TableMetadataBuilder {
public:
	TableMetadataBuilder();
	TableMetadataBuilder &SetFormatVersion(int32_t value);
	TableMetadataBuilder &SetTableUuid(string value);
	TableMetadataBuilder &SetLocation(string value);
	TableMetadataBuilder &SetLastUpdatedMs(int64_t value);
	TableMetadataBuilder &SetNextRowId(int64_t value);
	TableMetadataBuilder &SetProperties(case_insensitive_map_t<string> value);
	TableMetadataBuilder &SetSchemas(vector<Schema> value);
	TableMetadataBuilder &SetCurrentSchemaId(int32_t value);
	TableMetadataBuilder &SetLastColumnId(int32_t value);
	TableMetadataBuilder &SetPartitionSpecs(vector<PartitionSpec> value);
	TableMetadataBuilder &SetDefaultSpecId(int32_t value);
	TableMetadataBuilder &SetLastPartitionId(int32_t value);
	TableMetadataBuilder &SetSortOrders(vector<SortOrder> value);
	TableMetadataBuilder &SetDefaultSortOrderId(int32_t value);
	TableMetadataBuilder &SetEncryptionKeys(vector<EncryptedKey> value);
	TableMetadataBuilder &SetSnapshots(vector<Snapshot> value);
	TableMetadataBuilder &SetRefs(SnapshotReferences value);
	TableMetadataBuilder &SetCurrentSnapshotId(int64_t value);
	TableMetadataBuilder &SetLastSequenceNumber(int64_t value);
	TableMetadataBuilder &SetSnapshotLog(SnapshotLog value);
	TableMetadataBuilder &SetMetadataLog(MetadataLog value);
	TableMetadataBuilder &SetStatistics(vector<StatisticsFile> value);
	TableMetadataBuilder &SetPartitionStatistics(vector<PartitionStatisticsFile> value);
	optional<string> TryBuild(optional<TableMetadata> &result);
	TableMetadata Build();

private:
	optional<int32_t> format_version_;
	optional<string> table_uuid_;
	optional<string> location_;
	optional<int64_t> last_updated_ms_;
	optional<int64_t> next_row_id_;
	optional<case_insensitive_map_t<string>> properties_;
	optional<vector<Schema>> schemas_;
	optional<int32_t> current_schema_id_;
	optional<int32_t> last_column_id_;
	optional<vector<PartitionSpec>> partition_specs_;
	optional<int32_t> default_spec_id_;
	optional<int32_t> last_partition_id_;
	optional<vector<SortOrder>> sort_orders_;
	optional<int32_t> default_sort_order_id_;
	optional<vector<EncryptedKey>> encryption_keys_;
	optional<vector<Snapshot>> snapshots_;
	optional<SnapshotReferences> refs_;
	optional<int64_t> current_snapshot_id_;
	optional<int64_t> last_sequence_number_;
	optional<SnapshotLog> snapshot_log_;
	optional<MetadataLog> metadata_log_;
	optional<vector<StatisticsFile>> statistics_;
	optional<vector<PartitionStatisticsFile>> partition_statistics_;
	bool has_format_version_ = false;
	bool has_table_uuid_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
