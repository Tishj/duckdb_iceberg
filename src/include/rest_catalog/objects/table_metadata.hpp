
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
	TableMetadata(const TableMetadata &) = delete;
	TableMetadata &operator=(const TableMetadata &) = delete;
	TableMetadata(TableMetadata &&) = default;
	TableMetadata &operator=(TableMetadata &&) = default;

private:
	friend class TableMetadataBuilder;
	friend class GeneratedObjectAccess;
	TableMetadata();

public:
	// Deserialization
	static TableMetadata FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	TableMetadata Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t format_version;
	string table_uuid;
	optional<string> location;
	optional<int64_t> last_updated_ms;
	optional<int64_t> next_row_id;
	optional<case_insensitive_map_t<string>> properties;
	optional<vector<Schema>> schemas;
	optional<int32_t> current_schema_id;
	optional<int32_t> last_column_id;
	optional<vector<PartitionSpec>> partition_specs;
	optional<int32_t> default_spec_id;
	optional<int32_t> last_partition_id;
	optional<vector<SortOrder>> sort_orders;
	optional<int32_t> default_sort_order_id;
	optional<vector<EncryptedKey>> encryption_keys;
	optional<vector<Snapshot>> snapshots;
	optional<SnapshotReferences> refs;
	optional<int64_t> current_snapshot_id;
	optional<int64_t> last_sequence_number;
	optional<SnapshotLog> snapshot_log;
	optional<MetadataLog> metadata_log;
	optional<vector<StatisticsFile>> statistics;
	optional<vector<PartitionStatisticsFile>> partition_statistics;
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
	string TryBuild(TableMetadata &result);
	TableMetadata Build();

private:
	TableMetadata result_;
	bool has_format_version_ = false;
	bool has_table_uuid_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
