#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/table_function_catalog_entry.hpp"
#include "duckdb/common/enums/join_type.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/tableref/joinref.hpp"
#include "duckdb/common/enums/joinref_type.hpp"
#include "duckdb/common/enums/tableref_type.hpp"
#include "duckdb/parser/tableref/table_function_ref.hpp"
#include "duckdb/parser/query_node/recursive_cte_node.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/parser/expression/conjunction_expression.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"
#include "duckdb/parser/expression/comparison_expression.hpp"
#include "duckdb/parser/expression/star_expression.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"
#include "duckdb/parser/tableref/emptytableref.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/operator/logical_comparison_join.hpp"
#include "duckdb/common/file_opener.hpp"
#include "duckdb/common/file_system.hpp"
#include "duckdb/common/types/uuid.hpp"
#include "duckdb/common/numeric_utils.hpp"
#include "duckdb/common/printer.hpp"
#include "duckdb/common/sql_identifier.hpp"
#include "duckdb/common/operator/cast_operators.hpp"
#include "duckdb/common/crypto/md5.hpp"
#include "duckdb/parser/qualified_name.hpp"

#include "function/iceberg_functions.hpp"
#include "common/iceberg_utils.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/transaction/iceberg_transaction.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/iceberg_schema_set.hpp"
#include "catalog/rest/iceberg_table_set.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table_entry.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table_information.hpp"

#include "core/metadata/iceberg_table_metadata.hpp"

#include "function/ducklake/ducklake_column_stats.hpp"
#include "function/ducklake/ducklake_column.hpp"
#include "function/ducklake/ducklake_data_file.hpp"
#include "function/ducklake/ducklake_delete_file.hpp"
#include "function/ducklake/ducklake_metadata_serializer.hpp"
#include "function/ducklake/ducklake_partition_column.hpp"
#include "function/ducklake/ducklake_partition.hpp"
#include "function/ducklake/ducklake_schema.hpp"
#include "function/ducklake/ducklake_snapshot.hpp"
#include "function/ducklake/ducklake_table.hpp"
#include "function/ducklake/ducklake_utils.hpp"

namespace duckdb {

namespace iceberg {

namespace ducklake {

namespace {

struct DuckLakeSchemaVersionIntermediate {
	idx_t snapshot_id;
	idx_t schema_version;
	unordered_set<string> table_uuids;
};

} // namespace

static void SchemaToColumnsInternal(const vector<unique_ptr<IcebergColumnDefinition>> &columns,
                                    unordered_map<int64_t, DuckLakeColumn> &result,
                                    optional_ptr<const IcebergColumnDefinition> parent) {
	for (idx_t i = 0; i < columns.size(); i++) {
		auto &column = *columns[i];
		result.emplace(column.id, DuckLakeColumn(column, i, parent));
		if (!column.GetChildCount()) {
			continue;
		}
		SchemaToColumnsInternal(column.GetChildren(), result, column);
	}
}

static unordered_map<int64_t, DuckLakeColumn> SchemaToColumns(const IcebergTableSchema &schema) {
	unordered_map<int64_t, DuckLakeColumn> result;
	SchemaToColumnsInternal(schema.columns, result, nullptr);
	return result;
}

static string GetNumericStats(const unordered_map<int32_t, int64_t> &stats, int64_t column_id) {
	auto it = stats.find(column_id);
	if (it == stats.end()) {
		return "NULL";
	}
	return to_string(it->second);
}

struct IcebergToDuckLakeBindData : public TableFunctionData {
public:
	IcebergToDuckLakeBindData() {
	}

public:
	enum class ScopeType : uint8_t { CATALOG, SCHEMA, TABLE };

	static vector<reference<const IcebergSnapshot>> GetCurrentSnapshotLineage(const IcebergTableMetadata &metadata) {
		vector<reference<const IcebergSnapshot>> result;
		if (!metadata.current_snapshot_id) {
			return result;
		}

		unordered_set<int64_t> visited;
		auto snapshot_id = *metadata.current_snapshot_id;
		while (true) {
			if (!visited.insert(snapshot_id).second) {
				throw InvalidConfigurationException("Cycle detected in Iceberg snapshot lineage for table '%s'",
				                                    metadata.table_uuid);
			}
			auto snapshot = metadata.FindSnapshotByIdInternal(snapshot_id);
			if (!snapshot) {
				throw InvalidConfigurationException(
				    "Current Iceberg snapshot lineage for table '%s' references missing snapshot %lld",
				    metadata.table_uuid, snapshot_id);
			}
			result.emplace_back(*snapshot);
			if (!snapshot->parent_snapshot_id) {
				break;
			}
			snapshot_id = *snapshot->parent_snapshot_id;
		}
		std::reverse(result.begin(), result.end());
		return result;
	}

	void AddTable(IcebergTableInformation &table_info, ClientContext &context, const IcebergOptions &options) {
		auto &metadata = table_info.table_metadata;
		if (table_names_to_skip.count(table_info.name)) {
			//! FIXME: perhaps log that the table was skipped
			return;
		}

		vector<unique_ptr<IcebergTableMetadata>> historical_metadata;
		if (!metadata.metadata_log.empty()) {
			table_info.LoadCredentials(context);
			auto fs = make_shared_ptr<CachingFileSystemWrapper>(FileSystem::GetFileSystem(context), *context.db);
			for (auto &item : metadata.metadata_log) {
				auto parsed = IcebergTableMetadata::Parse(item.metadata_file, *fs, options.metadata_compression_codec);
				historical_metadata.push_back(
				    make_uniq<IcebergTableMetadata>(IcebergTableMetadata::FromTableMetadata(parsed)));
			}
		}

		struct ConversionEvent {
			timestamp_ms_t timestamp;
			optional_ptr<const IcebergTableMetadata> metadata;
			optional_ptr<const IcebergSnapshot> snapshot;
		};
		vector<ConversionEvent> events;
		for (auto &historical : historical_metadata) {
			events.push_back({historical->last_updated_ms, historical.get(), nullptr});
		}
		events.push_back({metadata.last_updated_ms, &metadata, nullptr});
		for (auto &snapshot_ref : GetCurrentSnapshotLineage(metadata)) {
			auto &snapshot = snapshot_ref.get();
			events.push_back({snapshot.timestamp_ms, nullptr, &snapshot});
		}
		std::stable_sort(events.begin(), events.end(), [](const ConversionEvent &left, const ConversionEvent &right) {
			if (left.timestamp != right.timestamp) {
				return left.timestamp < right.timestamp;
			}
			// Apply the table metadata state before processing the data snapshot produced by the same commit.
			return left.metadata && !right.metadata;
		});

		auto &schema_entry = table_info.schema;
		auto &schema = GetSchema(schema_entry.name.GetIdentifierName());

		auto &table = GetTable(table_info);
		schema.tables.push_back(table.table_uuid);
		table.schema_name = schema.schema_name;

		//! Current schema state
		optional_ptr<const IcebergTableSchema> last_schema;

		//! Current partition state
		optional_idx current_partition_spec_id;
		optional_ptr<DuckLakePartition> current_partition;

		auto apply_schema = [&](const IcebergTableSchema &current_schema, DuckLakeSnapshot &ducklake_snapshot) {
			auto current_columns = SchemaToColumns(current_schema);
			vector<DuckLakeColumn> added_columns;
			vector<int64_t> dropped_columns;
			if (last_schema) {
				if (last_schema->schema_id != current_schema.schema_id) {
					auto existing_columns = SchemaToColumns(*last_schema);
					for (auto &it : current_columns) {
						auto existing_it = existing_columns.find(it.first);
						if (existing_it == existing_columns.end() || existing_it->second != it.second) {
							added_columns.push_back(it.second);
						}
					}
					for (auto &it : existing_columns) {
						if (!current_columns.count(it.first)) {
							dropped_columns.push_back(it.first);
						}
					}
				}
			} else {
				for (auto &it : current_columns) {
					added_columns.push_back(it.second);
				}
			}
			for (auto &column : added_columns) {
				table.AddColumnVersion(column, ducklake_snapshot);
			}
			for (auto id : dropped_columns) {
				table.DropColumnVersion(id, ducklake_snapshot);
			}
			last_schema = current_schema;
		};

		for (auto &event : events) {
			auto timestamp = duckdb::Cast::Operation<timestamp_ms_t, timestamp_t>(event.timestamp);
			auto &ducklake_snapshot = GetSnapshot(timestamp);

			if (!table.has_snapshot) {
				//! Mark the table as being created by this snapshot
				table.catalog_id_offset = ducklake_snapshot.AddTable(table_info.table_metadata.table_uuid);
				table.start_snapshot = ducklake_snapshot.snapshot_time;
				table.has_snapshot = true;
			}

			if (event.metadata) {
				auto &metadata_state = *event.metadata;
				apply_schema(metadata_state.GetLatestSchema(), ducklake_snapshot);
				if (metadata_state.HasPartitionSpec() &&
				    (!current_partition_spec_id.IsValid() ||
				     current_partition_spec_id.GetIndex() != static_cast<idx_t>(metadata_state.default_spec_id))) {
					auto new_partition = make_uniq<DuckLakePartition>(metadata_state.GetLatestPartitionSpec());
					current_partition = table.AddPartition(std::move(new_partition), ducklake_snapshot);
					current_partition_spec_id = metadata_state.default_spec_id;
				}
				continue;
			}

			auto &snapshot = *event.snapshot;
			IcebergSnapshotScanInfo snapshot_info;
			snapshot_info.snapshot = snapshot;
			snapshot_info.schema_id = snapshot.GetSchemaId();
			apply_schema(*metadata.GetSchemaFromId(snapshot.GetSchemaId()), ducklake_snapshot);

			auto iceberg_manifest_list =
			    IcebergManifestList::Load(metadata.location, metadata, snapshot_info, context, options);

			vector<DuckLakeDataFile> new_data_files;
			vector<string> deleted_data_files;

			vector<DuckLakeDeleteFile> new_delete_files;
			vector<string> deleted_delete_files;
			for (auto &entry : iceberg_manifest_list->GetManifestFilesConst()) {
				auto &manifest = entry.file;
				auto &entries = entry.GetManifestEntries();

				if (!manifest.added_snapshot_id || *manifest.added_snapshot_id != snapshot.snapshot_id) {
					//! This is essentially an "EXISTING" manifest
					//! there just isn't a 'status' field to indicate that
					continue;
				}

				if (!current_partition_spec_id.IsValid() ||
				    static_cast<idx_t>(manifest.partition_spec_id) != current_partition_spec_id.GetIndex()) {
					auto &partition_spec = *metadata.FindPartitionSpecById(manifest.partition_spec_id);
					auto new_partition = make_uniq<DuckLakePartition>(partition_spec);
					current_partition = table.AddPartition(std::move(new_partition), ducklake_snapshot);
					current_partition_spec_id = manifest.partition_spec_id;
				}

				switch (manifest.content) {
				case IcebergManifestContentType::DATA: {
					for (auto &manifest_entry : entries) {
						auto &data_file = manifest_entry.data_file;
						D_ASSERT(data_file.content == IcebergManifestEntryContentType::DATA);
						if (manifest_entry.status == IcebergManifestEntryStatusType::EXISTING) {
							//! We don't care about existing entries
							continue;
						}
						if (manifest_entry.status == IcebergManifestEntryStatusType::ADDED) {
							new_data_files.push_back(
							    DuckLakeDataFile(manifest_entry, *current_partition, table.table_name));
						} else {
							D_ASSERT(manifest_entry.status == IcebergManifestEntryStatusType::DELETED);
							deleted_data_files.push_back(data_file.file_path);
						}
					}
					break;
				}
				case IcebergManifestContentType::DELETE: {
					unordered_map<string, string> existing_parquet_deletes;
					for (auto &manifest_entry : entries) {
						auto &data_file = manifest_entry.data_file;
						if (data_file.content == IcebergManifestEntryContentType::EQUALITY_DELETES) {
							throw InvalidInputException(
							    "Can't convert a table with equality deletes to a DuckLake table");
						}

						if (manifest_entry.status == IcebergManifestEntryStatusType::DELETED) {
							deleted_delete_files.push_back(data_file.file_path);
							continue;
						}
						auto delete_file = DuckLakeDeleteFile(manifest_entry, table.table_name);
						if (manifest_entry.status == IcebergManifestEntryStatusType::EXISTING) {
							if (delete_file.file_format == "parquet") {
								//! In Iceberg we ignore remnant parquet deletes, but we have to delete them for
								//! DuckLake
								existing_parquet_deletes.emplace(delete_file.data_file_path, delete_file.path);
							}
							continue;
						} else {
							D_ASSERT(manifest_entry.status == IcebergManifestEntryStatusType::ADDED);
							if (delete_file.file_format == "puffin") {
								//! A deletion vector!, check if there's a parquet file referencing the same data file
								//! that has to be invalidated
								auto it = existing_parquet_deletes.find(delete_file.data_file_path);
								if (it != existing_parquet_deletes.end()) {
									//! There is: delete it
									deleted_delete_files.push_back(it->second);
								}
							}
							new_delete_files.push_back(delete_file);
						}
					}
					break;
				}
				}
			}

			//! Process changes to delete files
			for (auto &path : deleted_delete_files) {
				table.DeleteDeleteFile(path, ducklake_snapshot);
			}
			for (auto &delete_file : new_delete_files) {
				table.AddDeleteFile(delete_file, ducklake_snapshot);
			}

			//! Process changes to data files
			for (auto &data_file : new_data_files) {
				table.AddDataFile(data_file, ducklake_snapshot);
			}
			for (auto &path : deleted_data_files) {
				table.DeleteDataFile(path, ducklake_snapshot);
			}
		}
	}
	void AssignSchemaBeginSnapshots() {
		//! Figure out in which snapshot the schemas were created
		vector<reference<DuckLakeSchema>> ordered_schemas;
		for (auto &entry : schemas) {
			ordered_schemas.push_back(entry.second);
		}
		std::sort(ordered_schemas.begin(), ordered_schemas.end(),
		          [](DuckLakeSchema &left, DuckLakeSchema &right) { return left.schema_name < right.schema_name; });
		for (auto &schema_ref : ordered_schemas) {
			auto &schema = schema_ref.get();
			if (schema.tables.empty()) {
				//! We can't serialize this, we have no clue when it was added
				continue;
			}
			schema.AssignEarliestSnapshot(tables, snapshots);
		}
	}

public:
	vector<string> CreateSQLStatements() {
		//! Order to process in:
		// - snapshot + schema_versions
		// - schema
		// - table
		//   - partition_info
		//     - partition_column
		//   - data_file
		//     - file_column_statistics
		//     - file_partition_value
		//   - delete_file
		// - table_stats
		// - snapshot_changes

		DuckLakeMetadataSerializer serializer;
		vector<string> sql;

		sql.push_back("BEGIN TRANSACTION;");
		sql.push_back("DELETE FROM {METADATA_CATALOG}.ducklake_table;");
		sql.push_back("DELETE FROM {METADATA_CATALOG}.ducklake_snapshot;");
		sql.push_back("DELETE FROM {METADATA_CATALOG}.ducklake_snapshot_changes;");

		const auto SCHEMA_VERSION_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_schema_versions VALUES(
				%llu, -- begin_snapshot
				%llu, -- schema_version
				%llu -- table_id
			);
		)";

		const auto FILE_COLUMN_STATS_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_file_column_stats VALUES(
				%d, -- data_file_id
				%d, -- table_id
				%d, -- column_id
				%s, -- column_size_bytes
				%s, -- value_count
				%s, -- null_count
				%s, -- min_value
				%s, -- max_value
				%s, -- contains_nan
				%s -- extra_stats
			);
		)";

		const auto FILE_PARTITION_VALUE_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_file_partition_value VALUES(
				%d, -- data_file_id
				%d, -- table_id
				%d, -- partition_key_index
				%s -- partition_value
			);
		)";

		const auto TABLE_STATS_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_table_stats VALUES(
				%d, -- table_id
				%d, -- record_count
				%d, -- next_row_id
				%d -- file_size_bytes
			);
		)";

		const auto TABLE_COLUMN_STATS_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_table_column_stats VALUES(
				%d, -- table_id
				%d, -- column_id
				%s, -- contains_null
				%s, -- contains_nan
				%s, -- min_value
				%s, -- max_value
				%s -- extra_stats
			);
		)";

		const auto SNAPSHOT_CHANGES_SQL = R"(
			INSERT INTO {METADATA_CATALOG}.ducklake_snapshot_changes VALUES(
				%d, -- snapshot_id
				'%s', -- changes_made
				%s, -- author
				%s, -- commit_message
				%s -- commit_extra_info
			);
		)";

		vector<DuckLakeSchemaVersionIntermediate> schema_versions;
		//! ducklake_snapshot
		for (auto &it : snapshots) {
			auto &snapshot = it.second;

			auto insert_statement = snapshot.FinalizeEntry(serializer);
			if (snapshot.catalog_changes) {
				auto snapshot_id = snapshot.snapshot_id.GetIndex();
				auto schema_version = snapshot.base_schema_version;
				DuckLakeSchemaVersionIntermediate new_schema_version;
				new_schema_version.snapshot_id = snapshot_id;
				new_schema_version.schema_version = schema_version;
				new_schema_version.table_uuids.insert(snapshot.created_table.begin(), snapshot.created_table.end());
				new_schema_version.table_uuids.insert(snapshot.altered_table.begin(), snapshot.altered_table.end());
				//! Push a new schema version
				schema_versions.push_back(new_schema_version);
			}
			sql.push_back(insert_statement);
		}

		//! ducklake_schema
		vector<reference<DuckLakeSchema>> ordered_schemas;
		for (auto &entry : schemas) {
			ordered_schemas.push_back(entry.second);
		}
		std::sort(ordered_schemas.begin(), ordered_schemas.end(), [&](DuckLakeSchema &left, DuckLakeSchema &right) {
			auto left_id = snapshots.at(left.start_snapshot).base_catalog_id + left.catalog_id_offset;
			auto right_id = snapshots.at(right.start_snapshot).base_catalog_id + right.catalog_id_offset;
			return left_id < right_id;
		});
		for (auto &schema_ref : ordered_schemas) {
			auto &schema = schema_ref.get();

			if (schema.tables.empty()) {
				//! We can't serialize this schema, it has no entries, so we can't date it back to any snapshot
				//! FIXME: we *could* assign it to the earliest snapshot in existence???
				continue;
			}
			auto insert_statement = schema.FinalizeEntry(snapshots);
			sql.push_back(insert_statement);
		}

		//! ducklake_table. Serializing by stable table id also keeps generated partition ids stable across increments.
		vector<reference<DuckLakeTable>> ordered_tables;
		for (auto &entry : tables) {
			ordered_tables.push_back(entry.second);
		}
		std::sort(ordered_tables.begin(), ordered_tables.end(), [&](DuckLakeTable &left, DuckLakeTable &right) {
			auto left_id = snapshots.at(left.start_snapshot).base_catalog_id + left.catalog_id_offset;
			auto right_id = snapshots.at(right.start_snapshot).base_catalog_id + right.catalog_id_offset;
			return left_id < right_id;
		});
		for (auto &table_ref : ordered_tables) {
			auto &table = table_ref.get();

			auto &schema = schemas.at(table.schema_name);
			auto schema_id = schema.schema_id.GetIndex();
			auto insert_statement = table.FinalizeEntry(schema_id, snapshots);
			sql.push_back(insert_statement);

			int64_t table_id = table.table_id.GetIndex();
			//! ducklake_partition_info
			for (auto &partition : table.all_partitions) {
				auto insert_statement = partition->FinalizeEntry(table_id, serializer, snapshots);
				sql.push_back(insert_statement);
				D_ASSERT(partition->partition_id.IsValid());
				auto partition_id = partition->partition_id.GetIndex();
				//! ducklake_partition_column
				for (idx_t i = 0; i < partition->columns.size(); i++) {
					auto &column = partition->columns[i];
					auto insert_statement = column.FinalizeEntry(table_id, partition_id, i);
					sql.push_back(insert_statement);
				}
			}

			//! ducklake_column
			for (auto &column : table.all_columns) {
				auto insert_statement = column.FinalizeEntry(table_id, snapshots);
				sql.push_back(insert_statement);
			}

			unordered_map<int32_t, DuckLakeColumnStats> column_stats;

			//! ducklake_data_file
			for (auto &data_file : table.all_data_files) {
				auto insert_statement = data_file.FinalizeEntry(table_id, snapshots);
				sql.push_back(insert_statement);

				auto data_file_id = data_file.data_file_id.GetIndex();
				auto &start_snapshot = snapshots.at(data_file.start_snapshot);

				//! ducklake_file_column_stats
				auto columns = table.GetColumnsAtSnapshot(start_snapshot);
				for (auto &it : columns) {
					auto column_id = it.first;
					auto &column = it.second.get();
					auto &manifest_entry = data_file.manifest_entry;
					auto &iceberg_data_file = manifest_entry.data_file;

					auto column_size_bytes = GetNumericStats(iceberg_data_file.column_sizes, column_id);
					auto value_count = GetNumericStats(iceberg_data_file.value_counts, column_id);

					Value lower_bound;
					Value upper_bound;
					Value null_count;

					auto lower_bound_it = iceberg_data_file.lower_bounds.find(column.column_id);
					auto upper_bound_it = iceberg_data_file.upper_bounds.find(column.column_id);
					if (lower_bound_it != iceberg_data_file.lower_bounds.end()) {
						lower_bound = lower_bound_it->second;
					}
					if (upper_bound_it != iceberg_data_file.upper_bounds.end()) {
						upper_bound = upper_bound_it->second;
					}

					LogicalType logical_type;
					if (!column.IsNested()) {
						logical_type = DuckLakeUtils::FromStringBaseType(column.column_type);
					} else {
						logical_type = LogicalType::VARCHAR;
					}

					//! Transform the stats stored in the iceberg metadata
					auto stats = IcebergPredicateStats::DeserializeBounds(lower_bound, upper_bound, column.column_name,
					                                                      logical_type);
					auto null_counts_it = iceberg_data_file.null_value_counts.find(column.column_id);
					if (null_counts_it != iceberg_data_file.null_value_counts.end()) {
						null_count = null_counts_it->second;
						stats.has_null = null_count != 0;
					}
					auto nan_counts_it = iceberg_data_file.nan_value_counts.find(column.column_id);
					if (nan_counts_it != iceberg_data_file.nan_value_counts.end()) {
						auto &nan_count = nan_counts_it->second;
						stats.has_nan = nan_count != 0;
					}

					auto contains_nan = stats.has_nan ? "true" : "false";
					auto min_value = stats.lower_bound->IsNull() ? "NULL" : "'" + stats.lower_bound->ToString() + "'";
					auto max_value = stats.upper_bound->IsNull() ? "NULL" : "'" + stats.upper_bound->ToString() + "'";

					auto insert_statement = StringUtil::Format(FILE_COLUMN_STATS_SQL,
					                                           // data_file_id
					                                           data_file_id,
					                                           // table_id
					                                           table_id,
					                                           // column_id
					                                           column_id,
					                                           // column_size_bytes
					                                           column_size_bytes,
					                                           // value_count
					                                           value_count,
					                                           // null_count
					                                           null_count.ToString(),
					                                           // min_value
					                                           min_value,
					                                           // max_value
					                                           max_value,
					                                           // contains_nan
					                                           contains_nan,
					                                           // extra_stats
					                                           "NULL");
					sql.push_back(insert_statement);

					if (!data_file.has_end && !column.has_end && !column.IsNested()) {
						//! This data file is currently active, collect stats for it
						auto file_stats_it = column_stats.find(column_id);
						if (file_stats_it == column_stats.end()) {
							file_stats_it = column_stats.emplace(column_id, column).first;
						}
						auto &file_column_stats = file_stats_it->second;
						file_column_stats.AddStats(stats);
					}
				}

				//! ducklake_file_partition_value
				auto &partition_info = data_file.manifest_entry.data_file.partition_info;
				auto &partition = data_file.partition;

				// Build a map from partition_field_id to DataFilePartitionInfo for quick lookup
				unordered_map<uint64_t, reference<const IcebergPartitionInfo>> field_id_to_info;
				for (auto &pi : partition_info) {
					field_id_to_info.emplace(pi.field_id, pi);
				}

				for (idx_t partition_key_index = 0; partition_key_index < partition.columns.size();
				     partition_key_index++) {
					auto &partition_column = partition.columns[partition_key_index];

					auto partition_it = field_id_to_info.find(partition_column.partition_field_id);
					string partition_value;
					if (partition_it == field_id_to_info.end()) {
						partition_value = "NULL";
					} else {
						partition_value = "'" + partition_it->second.get().value.ToString() + "'";
					}
					auto values = StringUtil::Format(FILE_PARTITION_VALUE_SQL,
					                                 // data_file_id
					                                 data_file_id,
					                                 // table_id
					                                 table_id,
					                                 // partition_key_index
					                                 partition_key_index,
					                                 // partition_value
					                                 partition_value);
					sql.push_back(StringUtil::Format(" %s", values));
				}
			}

			//! ducklake_delete_file
			for (auto &delete_file : table.all_delete_files) {
				auto insert_statement = delete_file.FinalizeEntry(table_id, table.all_data_files, snapshots);
				sql.push_back(insert_statement);
			}

			//! ducklake_table_stats
			idx_t record_count = 0;
			idx_t file_size_bytes = 0;

			for (auto &it : table.current_data_files) {
				auto &data_file = table.all_data_files[it.second];

				record_count += data_file.record_count;
				file_size_bytes += data_file.file_size_bytes;
			}
			for (auto &it : table.current_delete_files) {
				auto &delete_file = table.all_delete_files[it.second];

				record_count -= delete_file.record_count;
				auto &data_file = table.all_data_files[delete_file.referenced_data_file];
				D_ASSERT(!data_file.has_end);
				auto percent_deleted = double(delete_file.record_count) / (data_file.record_count / 100.00);
				file_size_bytes -= LossyNumericCast<idx_t>(double(data_file.file_size_bytes) / percent_deleted);
			}

			if (!column_stats.empty()) {
				//! FIXME: for v2 compatibility this uses the 'record_count' as the 'next_row_id'
				auto insert_statement = StringUtil::Format(TABLE_STATS_SQL,
				                                           // table_id
				                                           table_id,
				                                           // record_count
				                                           record_count,
				                                           // next_row_id
				                                           record_count,
				                                           // file_size_bytes
				                                           file_size_bytes);
				sql.push_back(insert_statement);
			}

			//! ducklake_table_column_stats
			for (auto &it : column_stats) {
				auto column_id = it.first;
				auto &stats = it.second;

				auto contains_null = stats.contains_null ? "true" : "false";
				auto contains_nan = stats.contains_nan ? "true" : "false";
				auto min_value = stats.min_value.IsNull() ? "NULL" : "'" + stats.min_value.ToString() + "'";
				auto max_value = stats.max_value.IsNull() ? "NULL" : "'" + stats.max_value.ToString() + "'";
				auto insert_statement = StringUtil::Format(TABLE_COLUMN_STATS_SQL,
				                                           // table_id
				                                           table_id,
				                                           // column_id
				                                           column_id,
				                                           // contains_null
				                                           contains_null,
				                                           // contains_nan
				                                           contains_nan,
				                                           // min_value
				                                           min_value,
				                                           // max_value
				                                           max_value,
				                                           // extra_stats
				                                           "NULL");
				sql.push_back(insert_statement);
			}
		}

		//! ducklake_schema_version
		for (auto &item : schema_versions) {
			auto &snapshot_id = item.snapshot_id;
			auto &schema_version = item.schema_version;
			for (auto &table_uuid : item.table_uuids) {
				auto table_it = tables.find(table_uuid);
				if (table_it == tables.end()) {
					throw InternalException(
					    "Corrupt snapshot detected, created/modified table '%s' but no table with that uuid exists",
					    table_uuid);
				}
				auto &table = table_it->second;
				auto table_id = table.table_id.GetIndex();

				sql.push_back(StringUtil::Format(SCHEMA_VERSION_SQL,
				                                 //! begin_snapshot
				                                 snapshot_id,
				                                 //! schema_version
				                                 schema_version,
				                                 //! table_id
				                                 table_id));
			}
		}

		//! ducklake_snapshot_changes
		for (auto &it : snapshots) {
			auto &snapshot = it.second;

			vector<string> changes;
			for (auto &schema_name : snapshot.created_schema) {
				auto escaped_name = SQLString::ToString(schema_name);

				changes.push_back(StringUtil::Format("created_schema:%s", escaped_name));
			}

			for (auto &table_uuid : snapshot.created_table) {
				auto &table = tables.at(table_uuid);

				auto schema_name = SQLString::ToString(table.schema_name);
				auto table_name = SQLString::ToString(table.table_name);

				changes.push_back(StringUtil::Format("created_table:%s.%s", schema_name, table_name));
			}

			for (auto &table_uuid : snapshot.inserted_into_table) {
				auto &table = tables.at(table_uuid);

				auto table_id = table.table_id.GetIndex();
				changes.push_back(StringUtil::Format("inserted_into_table:%d", table_id));
			}

			for (auto &table_uuid : snapshot.deleted_from_table) {
				auto &table = tables.at(table_uuid);

				auto table_id = table.table_id.GetIndex();
				changes.push_back(StringUtil::Format("deleted_from_table:%d", table_id));
			}

			for (auto &schema_name : snapshot.dropped_schema) {
				auto &schema = schemas.at(schema_name);
				auto schema_id = schema.schema_id.GetIndex();

				changes.push_back(StringUtil::Format("dropped_schema:%d", schema_id));
			}

			for (auto &table_uuid : snapshot.dropped_table) {
				auto &table = tables.at(table_uuid);

				auto table_id = table.table_id.GetIndex();
				changes.push_back(StringUtil::Format("dropped_table:%d", table_id));
			}

			for (auto &table_uuid : snapshot.altered_table) {
				if (snapshot.created_table.count(table_uuid)) {
					//! Table was created in this snapshot,
					//! any alters to the table made as part of that creation don't have to be recorded as
					//! 'snapshot_changes'
					continue;
				}
				auto &table = tables.at(table_uuid);

				auto table_id = table.table_id.GetIndex();
				changes.push_back(StringUtil::Format("altered_table:%d", table_id));
			}
			std::sort(changes.begin(), changes.end());
			auto snapshot_id = snapshot.snapshot_id.GetIndex();
			auto insert_statement = StringUtil::Format(SNAPSHOT_CHANGES_SQL,
			                                           // snapshot_id
			                                           snapshot_id,
			                                           // changes_made
			                                           StringUtil::Join(changes, ","),
			                                           // author
			                                           "NULL",
			                                           // commit_message
			                                           "NULL",
			                                           // commit_extra_info
			                                           "NULL");
			sql.push_back(insert_statement);
		}
		sql.push_back("COMMIT TRANSACTION;");

		return sql;
	}

private:
	DuckLakeSnapshot &GetSnapshot(timestamp_t timestamp) {
		auto it = snapshots.find(timestamp);
		if (it != snapshots.end()) {
			return it->second;
		}
		auto res = snapshots.emplace(timestamp, DuckLakeSnapshot(timestamp));
		return res.first->second;
	}

	DuckLakeTable &GetTable(const IcebergTableInformation &table_info) {
		auto &metadata = table_info.table_metadata;
		auto table_uuid = metadata.table_uuid;
		auto it = tables.find(table_uuid);
		if (it != tables.end()) {
			return it->second;
		}
		auto res = tables.emplace(table_uuid, DuckLakeTable(table_uuid, table_info.name));
		return res.first->second;
	}

	DuckLakeSchema &GetSchema(const string &schema_name) {
		auto it = schemas.find(schema_name);
		if (it != schemas.end()) {
			return it->second;
		}
		auto res = schemas.emplace(schema_name, DuckLakeSchema(schema_name));
		return res.first->second;
	}

public:
	//! timestamp -> snapshot
	map<timestamp_t, DuckLakeSnapshot> snapshots;
	//! table_uuid -> table
	unordered_map<string, DuckLakeTable> tables;
	//! schema name -> schema
	unordered_map<string, DuckLakeSchema> schemas;

public:
	//! Skip these tables (should be set if a table doesn't meet the conversion criteria)
	set<string> table_names_to_skip;

public:
	//! The statements to execute on the metadata catalog
	vector<string> sql_statements;
	string ducklake_catalog;
	string iceberg_catalog;
	ScopeType scope = ScopeType::CATALOG;
	string selected_schema;
	string selected_table;
	string source_identity;
	string scope_identity;
};

static unique_ptr<FunctionData> IcebergToDuckLakeBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
	auto ret = make_uniq<IcebergToDuckLakeBindData>();
	auto input_string = input.inputs[0].ToString();
	ret->iceberg_catalog = input_string;
	ret->ducklake_catalog = input.inputs[1].ToString();

	auto &catalog = Catalog::GetCatalog(context, Identifier(input_string));
	auto catalog_type = catalog.GetCatalogType();
	if (catalog_type != "iceberg") {
		throw InvalidInputException("First parameter must be the name of an attached Iceberg catalog");
	}
	auto &iceberg_catalog = catalog.Cast<IcebergCatalog>();
	ret->source_identity =
	    StringUtil::Format("%s|%s|%s", iceberg_catalog.uri, iceberg_catalog.GetWarehouse(), iceberg_catalog.prefix);
	auto &schema_set = iceberg_catalog.GetSchemas();

	IcebergOptions options(input.named_parameters);
	for (auto &kv : input.named_parameters) {
		auto loption = StringUtil::Lower(kv.first.GetIdentifierName());
		if (loption == "skip_tables") {
			auto &type = kv.second.type();
			if (kv.second.IsNull() || type.id() != LogicalTypeId::LIST) {
				throw InvalidInputException("'skip_tables' has to be provided as a list of strings");
			}
			auto &child_type = ListType::GetChildType(type);
			if (child_type.id() != LogicalTypeId::VARCHAR) {
				throw InvalidInputException("'skip_tables' has to be provided as a list of strings");
			}
			auto &tables = ListValue::GetChildren(kv.second);
			for (auto &table : tables) {
				ret->table_names_to_skip.insert(table.GetValue<string>());
			}
		} else if (loption == "schema") {
			if (ret->scope != IcebergToDuckLakeBindData::ScopeType::CATALOG) {
				throw InvalidInputException("'schema' and 'table' are mutually exclusive");
			}
			ret->scope = IcebergToDuckLakeBindData::ScopeType::SCHEMA;
			ret->selected_schema = kv.second.GetValue<string>();
			if (ret->selected_schema.empty()) {
				throw InvalidInputException("'schema' cannot be empty");
			}
		} else if (loption == "table") {
			if (ret->scope != IcebergToDuckLakeBindData::ScopeType::CATALOG) {
				throw InvalidInputException("'schema' and 'table' are mutually exclusive");
			}
			auto components = QualifiedName::ParseComponents(kv.second.GetValue<string>());
			if (components.size() < 2) {
				throw InvalidInputException("'table' must be qualified as 'schema.table'");
			}
			ret->scope = IcebergToDuckLakeBindData::ScopeType::TABLE;
			ret->selected_table = components.back().GetIdentifierName();
			components.pop_back();
			ret->selected_schema = StringUtil::Join(components, ".");
		}
	}

	schema_set.LoadEntries(context);
	vector<reference<IcebergSchemaEntry>> selected_schemas;
	for (auto &entry : schema_set.GetEntries()) {
		auto &schema_entry = entry.second->Cast<IcebergSchemaEntry>();
		if (ret->scope != IcebergToDuckLakeBindData::ScopeType::CATALOG &&
		    schema_entry.name.GetIdentifierName() != ret->selected_schema) {
			continue;
		}
		selected_schemas.push_back(schema_entry);
	}
	std::sort(selected_schemas.begin(), selected_schemas.end(),
	          [](IcebergSchemaEntry &left, IcebergSchemaEntry &right) {
		          return left.name.GetIdentifierName() < right.name.GetIdentifierName();
	          });

	bool found_table = false;
	for (auto &schema_ref : selected_schemas) {
		auto &schema_entry = schema_ref.get();
		auto &tables = schema_entry.tables;
		tables.LoadEntries(context);
		vector<reference<IcebergTableInformation>> selected_tables;
		for (auto &entry : tables.GetEntriesMutable()) {
			auto &table = entry.second;
			if (ret->scope == IcebergToDuckLakeBindData::ScopeType::TABLE && table->name != ret->selected_table) {
				continue;
			}
			tables.FillEntry(context, *table);
			selected_tables.push_back(*table);
		}
		std::sort(selected_tables.begin(), selected_tables.end(),
		          [](IcebergTableInformation &left, IcebergTableInformation &right) { return left.name < right.name; });
		for (auto &table_ref : selected_tables) {
			found_table = true;
			ret->AddTable(table_ref.get(), context, options);
		}
	}
	if (ret->scope == IcebergToDuckLakeBindData::ScopeType::SCHEMA && selected_schemas.empty()) {
		throw InvalidInputException("Iceberg schema '%s' does not exist", ret->selected_schema);
	}
	if (ret->scope == IcebergToDuckLakeBindData::ScopeType::TABLE && !found_table) {
		throw InvalidInputException("Iceberg table '%s.%s' does not exist", ret->selected_schema, ret->selected_table);
	}
	if (ret->tables.empty()) {
		throw InvalidInputException("The selected Iceberg conversion scope contains no convertible tables");
	}
	switch (ret->scope) {
	case IcebergToDuckLakeBindData::ScopeType::CATALOG:
		ret->scope_identity = "catalog";
		break;
	case IcebergToDuckLakeBindData::ScopeType::SCHEMA:
		ret->scope_identity = StringUtil::Format("schema:%s", ret->selected_schema);
		break;
	case IcebergToDuckLakeBindData::ScopeType::TABLE:
		ret->scope_identity = StringUtil::Format("table:%s.%s", ret->selected_schema, ret->selected_table);
		break;
	}
	if (!ret->table_names_to_skip.empty()) {
		ret->scope_identity += "|skip=" + StringUtil::Join(ret->table_names_to_skip, ",");
	}

	ret->AssignSchemaBeginSnapshots();

	ret->sql_statements = ret->CreateSQLStatements();

	return_types.emplace_back(LogicalType::BIGINT);
	names.emplace_back("count");
	return std::move(ret);
}

} // namespace ducklake

} // namespace iceberg

struct IcebergToDuckLakeGlobalTableFunctionState : public GlobalTableFunctionState {
public:
	IcebergToDuckLakeGlobalTableFunctionState(unique_ptr<Connection> connection, const string &metadata_catalog)
	    : connection(std::move(connection)), metadata_catalog(metadata_catalog) {};
	virtual ~IcebergToDuckLakeGlobalTableFunctionState() {
		if (connection) {
			connection.reset();
		}
	}

public:
	static constexpr const char *OWNER_VERSION_KEY = "iceberg_to_ducklake_version";
	static constexpr const char *OWNER_SOURCE_KEY = "iceberg_to_ducklake_source";
	static constexpr const char *OWNER_SCOPE_KEY = "iceberg_to_ducklake_scope";
	static constexpr const char *OWNER_FINGERPRINT_KEY = "iceberg_to_ducklake_fingerprint";
	static constexpr const char *EXPECTED_CATALOG = "__iceberg_to_ducklake_expected";

	string QualifiedTable(const string &catalog, const string &schema, const string &table) const {
		return StringUtil::Format("%s.%s.%s", SQLIdentifier::ToString(catalog), SQLIdentifier::ToString(schema),
		                          SQLIdentifier::ToString(table));
	}

	vector<pair<string, string>> GetCatalogTables(const string &catalog) {
		auto query = StringUtil::Format(
		    "SELECT table_schema, table_name FROM information_schema.tables WHERE table_catalog = %s AND "
		    "table_type = 'BASE TABLE' ORDER BY table_schema, table_name",
		    SQLString::ToString(catalog));
		auto result = connection->Query(query);
		if (result->HasError()) {
			result->ThrowError("Failed to inspect DuckLake metadata tables: ");
		}
		vector<pair<string, string>> tables;
		while (auto chunk = result->Fetch()) {
			for (idx_t row = 0; row < chunk->size(); row++) {
				tables.emplace_back(chunk->GetValue(0, row).GetValue<string>(),
				                    chunk->GetValue(1, row).GetValue<string>());
			}
		}
		return tables;
	}

	string ComputeCatalogFingerprint(const string &catalog) {
		MD5Context md5;
		for (auto &entry : GetCatalogTables(catalog)) {
			auto &schema = entry.first;
			auto &table = entry.second;
			md5.Add(to_string(schema.size()));
			md5.Add(":");
			md5.Add(schema);
			md5.Add(to_string(table.size()));
			md5.Add(":");
			md5.Add(table);
			string where;
			if (table == "ducklake_metadata") {
				where = StringUtil::Format(" WHERE key NOT IN (%s, %s, %s, %s)", SQLString::ToString(OWNER_VERSION_KEY),
				                           SQLString::ToString(OWNER_SOURCE_KEY), SQLString::ToString(OWNER_SCOPE_KEY),
				                           SQLString::ToString(OWNER_FINGERPRINT_KEY));
			}
			auto query =
			    StringUtil::Format("SELECT * FROM %s%s ORDER BY ALL", QualifiedTable(catalog, schema, table), where);
			auto result = connection->Query(query);
			if (result->HasError()) {
				result->ThrowError("Failed to fingerprint DuckLake metadata catalog: ");
			}
			for (idx_t column = 0; column < result->ColumnCount(); column++) {
				md5.Add("<COLUMN>");
				md5.Add(result->ColumnName(column));
				md5.Add(result->types[column].ToString());
			}
			while (auto chunk = result->Fetch()) {
				for (idx_t row = 0; row < chunk->size(); row++) {
					md5.Add("<ROW>");
					for (idx_t column = 0; column < chunk->ColumnCount(); column++) {
						auto value = chunk->GetValue(column, row);
						if (value.IsNull()) {
							md5.Add("<NULL>");
						} else {
							auto string_value = value.ToString();
							md5.Add(to_string(string_value.size()));
							md5.Add(":");
							md5.Add(string_value);
						}
					}
				}
			}
		}
		return md5.FinishHex();
	}

	optional<string> GetOwnerValue(const string &key) {
		auto query = StringUtil::Format(
		    "SELECT value FROM %s.ducklake_metadata WHERE key = %s AND scope IS NULL AND scope_id IS NULL",
		    SQLIdentifier::ToString(metadata_catalog), SQLString::ToString(key));
		auto result = connection->Query(query);
		if (result->HasError()) {
			result->ThrowError("Failed to read iceberg_to_ducklake ownership metadata: ");
		}
		auto chunk = result->Fetch();
		if (!chunk || chunk->size() == 0) {
			return nullopt;
		}
		if (chunk->size() != 1 || result->Fetch()) {
			throw InvalidConfigurationException("Duplicate iceberg_to_ducklake ownership key '%s'", key);
		}
		return chunk->GetValue(0, 0).GetValue<string>();
	}

	void VerifyDuckLakeVersion() {
		auto version_query =
		    StringUtil::Replace("SELECT value FROM {METADATA_CATALOG}.ducklake_metadata where key = 'version'",
		                        "{METADATA_CATALOG}", metadata_catalog);
		auto result = connection->Query(version_query);
		if (result->HasError()) {
			result->ThrowError("'iceberg_to_ducklake' version verification query failed: ");
		}

		D_ASSERT(result->ColumnCount() == 1);
		auto chunk = result->Fetch();
		if (!chunk) {
			throw InvalidInputException("'iceberg_to_ducklake' version verification query failed, produced no chunks");
		}
		if (chunk->size() == 0) {
			throw InvalidInputException("Metadata catalog does not have a 'version' entry in 'ducklake_metadata'");
		}
		auto value = chunk->GetValue(0, 0);
		if (value.IsNull() || value.type().id() != LogicalTypeId::VARCHAR) {
			throw InvalidInputException(
			    "DuckLake version metadata is corrupt, the value can't be NULL and has to be of type VARCHAR");
		}
		auto version_string = value.GetValue<string>();
		if (!StringUtil::StartsWith(version_string, "0.4")) {
			throw InvalidInputException(
			    "'iceberg_to_ducklake' only support version 0.4 currently, detected '%s' instead", version_string);
		}
	}

	void LoadOwnership(const iceberg::ducklake::IcebergToDuckLakeBindData &bind_data) {
		auto version = GetOwnerValue(OWNER_VERSION_KEY);
		if (!version) {
			auto target = SQLIdentifier::ToString(metadata_catalog);
			auto ownership_count = connection->Query(
			    StringUtil::Format("SELECT count(*) FROM %s.ducklake_metadata WHERE key IN (%s, %s, %s, %s)", target,
			                       SQLString::ToString(OWNER_VERSION_KEY), SQLString::ToString(OWNER_SOURCE_KEY),
			                       SQLString::ToString(OWNER_SCOPE_KEY), SQLString::ToString(OWNER_FINGERPRINT_KEY)));
			if (ownership_count->HasError()) {
				ownership_count->ThrowError("Failed to inspect iceberg_to_ducklake ownership metadata: ");
			}
			auto ownership_chunk = ownership_count->Fetch();
			if (!ownership_chunk || ownership_chunk->GetValue(0, 0).GetValue<int64_t>() != 0) {
				throw InvalidConfigurationException("Incomplete iceberg_to_ducklake ownership metadata");
			}
			incremental = false;
			VerifyEmptyCatalog();
			return;
		}
		if (*version != "1") {
			throw InvalidConfigurationException("Unsupported iceberg_to_ducklake ownership version '%s'", *version);
		}
		auto source = GetOwnerValue(OWNER_SOURCE_KEY);
		auto scope = GetOwnerValue(OWNER_SCOPE_KEY);
		auto fingerprint = GetOwnerValue(OWNER_FINGERPRINT_KEY);
		if (!source || !scope || !fingerprint) {
			throw InvalidConfigurationException("Incomplete iceberg_to_ducklake ownership metadata");
		}
		if (*source != bind_data.source_identity) {
			throw InvalidInputException("DuckLake catalog is owned by a different Iceberg source");
		}
		if (*scope != bind_data.scope_identity) {
			throw InvalidInputException("Incremental conversion must use the same scope and skip list as the seed");
		}
		auto actual_fingerprint = ComputeCatalogFingerprint(metadata_catalog);
		if (actual_fingerprint != *fingerprint) {
			throw InvalidInputException("DuckLake catalog was altered after the previous iceberg_to_ducklake "
			                            "conversion; reseeding is required");
		}
		incremental = true;
	}

	void VerifyEmptyCatalog() {
		auto query = StringUtil::Replace("SELECT max(snapshot_id) FROM {METADATA_CATALOG}.ducklake_snapshot;",
		                                 "{METADATA_CATALOG}", metadata_catalog);
		auto result = connection->Query(query);
		if (result->HasError()) {
			result->ThrowError("'iceberg_to_ducklake' verification query failed: ");
		}

		D_ASSERT(result->ColumnCount() == 1);
		auto chunk = result->Fetch();
		if (!chunk) {
			throw InvalidInputException("'iceberg_to_ducklake' verification query failed, produced no chunks");
		}
		if (chunk->size() == 0) {
			throw InvalidInputException("Couldn't get 'max(snapshot_id)', produced 0 rows");
		}
		auto value = chunk->GetValue(0, 0);
		if (value.IsNull() || value.type().id() != LogicalTypeId::BIGINT) {
			throw InvalidInputException(
			    "'max(snapshot_id)' did not produce a non-null value, or the value type is not BIGINT (int64)");
		}
		auto max_snapshot_id = value.GetValue<int64_t>();
		if (max_snapshot_id != 0) {
			throw InvalidInputException("'iceberg_to_ducklake' can only seed a pristine DuckLake catalog");
		}

		for (auto &entry : GetCatalogTables(metadata_catalog)) {
			auto &table = entry.second;
			if (table == "ducklake_metadata") {
				continue;
			}
			auto table_name = QualifiedTable(metadata_catalog, entry.first, table);
			string predicate;
			bool require_single_row = false;
			if (table == "ducklake_snapshot" || table == "ducklake_snapshot_changes") {
				predicate = " WHERE snapshot_id <> 0";
				require_single_row = true;
			} else if (table == "ducklake_schema") {
				predicate =
				    " WHERE schema_id <> 0 OR schema_name <> 'main' OR begin_snapshot <> 0 OR end_snapshot IS NOT NULL";
				require_single_row = true;
			}
			auto count_result =
			    connection->Query(StringUtil::Format("SELECT count(*) FROM %s%s", table_name, predicate));
			if (count_result->HasError()) {
				count_result->ThrowError("Failed to verify pristine DuckLake metadata: ");
			}
			auto count_chunk = count_result->Fetch();
			if (!count_chunk || count_chunk->GetValue(0, 0).GetValue<int64_t>() != 0) {
				throw InvalidInputException("'iceberg_to_ducklake' can only seed a pristine DuckLake catalog");
			}
			if (require_single_row) {
				auto total_result = connection->Query(StringUtil::Format("SELECT count(*) FROM %s", table_name));
				if (total_result->HasError()) {
					total_result->ThrowError("Failed to verify pristine DuckLake metadata: ");
				}
				auto total_chunk = total_result->Fetch();
				if (!total_chunk || total_chunk->GetValue(0, 0).GetValue<int64_t>() != 1) {
					throw InvalidInputException("'iceberg_to_ducklake' can only seed a pristine DuckLake catalog");
				}
			}
		}
	}

	void ExecuteOrThrow(const string &query, const string &error_prefix) {
		auto result = connection->Query(query);
		if (result->HasError()) {
			result->ThrowError(error_prefix);
		}
	}

	int64_t GetMaxSnapshot(const string &catalog) {
		auto query =
		    StringUtil::Format("SELECT max(snapshot_id) FROM %s.ducklake_snapshot", SQLIdentifier::ToString(catalog));
		auto result = connection->Query(query);
		if (result->HasError()) {
			result->ThrowError("Failed to read DuckLake snapshot watermark: ");
		}
		auto chunk = result->Fetch();
		if (!chunk || chunk->size() != 1 || chunk->GetValue(0, 0).IsNull()) {
			throw InvalidConfigurationException("DuckLake snapshot metadata has no watermark");
		}
		return chunk->GetValue(0, 0).GetValue<int64_t>();
	}

	void MaterializeExpectedCatalog(const iceberg::ducklake::IcebergToDuckLakeBindData &bind_data) {
		ExecuteOrThrow(StringUtil::Format("ATTACH ':memory:' AS %s", SQLIdentifier::ToString(EXPECTED_CATALOG)),
		               "Failed to attach incremental conversion staging catalog: ");
		for (auto &entry : GetCatalogTables(metadata_catalog)) {
			auto &schema = entry.first;
			auto &table = entry.second;
			if (schema != "main") {
				ExecuteOrThrow(StringUtil::Format("CREATE SCHEMA IF NOT EXISTS %s.%s",
				                                  SQLIdentifier::ToString(EXPECTED_CATALOG),
				                                  SQLIdentifier::ToString(schema)),
				               "Failed to create staging schema: ");
			}
			auto target_table = QualifiedTable(metadata_catalog, schema, table);
			auto expected_table = QualifiedTable(EXPECTED_CATALOG, schema, table);
			ExecuteOrThrow(
			    StringUtil::Format("CREATE TABLE %s AS SELECT * FROM %s WHERE false", expected_table, target_table),
			    "Failed to create staging metadata table: ");
		}

		auto target = SQLIdentifier::ToString(metadata_catalog);
		auto expected = SQLIdentifier::ToString(EXPECTED_CATALOG);
		ExecuteOrThrow(
		    StringUtil::Format("INSERT INTO %s.ducklake_metadata SELECT * FROM %s.ducklake_metadata WHERE key NOT IN "
		                       "(%s, %s, %s, %s)",
		                       expected, target, SQLString::ToString(OWNER_VERSION_KEY),
		                       SQLString::ToString(OWNER_SOURCE_KEY), SQLString::ToString(OWNER_SCOPE_KEY),
		                       SQLString::ToString(OWNER_FINGERPRINT_KEY)),
		    "Failed to copy base DuckLake metadata into staging: ");
		ExecuteOrThrow(StringUtil::Format(
		                   "INSERT INTO %s.ducklake_schema SELECT * FROM %s.ducklake_schema WHERE schema_id = 0 AND "
		                   "schema_name = 'main'",
		                   expected, target),
		               "Failed to copy the base DuckLake schema into staging: ");

		auto query = StringUtil::Join(bind_data.sql_statements, "\n");
		query = StringUtil::Replace(query, "{METADATA_CATALOG}", expected);
		ExecuteOrThrow(query, "Failed to materialize expected Iceberg conversion state: ");

		// Iceberg namespaces do not have UUIDs. Preserve UUIDs assigned by the seed for schemas that already exist.
		ExecuteOrThrow(
		    StringUtil::Format(
		        "UPDATE %s.ducklake_schema AS expected_schema SET schema_uuid = target_schema.schema_uuid FROM "
		        "%s.ducklake_schema AS target_schema WHERE expected_schema.schema_id = target_schema.schema_id "
		        "AND expected_schema.begin_snapshot = target_schema.begin_snapshot",
		        expected, target),
		    "Failed to normalize staging schema UUIDs: ");
	}

	void VerifyExpectedPrefix() {
		auto target_max = GetMaxSnapshot(metadata_catalog);
		auto expected_max = GetMaxSnapshot(EXPECTED_CATALOG);
		if (expected_max < target_max) {
			throw InvalidInputException("Iceberg history no longer contains the previously converted DuckLake "
			                            "snapshots; reseeding is required");
		}
		auto target = SQLIdentifier::ToString(metadata_catalog);
		auto expected = SQLIdentifier::ToString(EXPECTED_CATALOG);
		auto verify_relation = [&](const string &name, const string &target_relation, const string &expected_relation) {
			auto mismatch_query =
			    StringUtil::Format(R"(
				SELECT count(*) FROM (
					(%s EXCEPT ALL %s)
					UNION ALL
					(%s EXCEPT ALL %s)
				) mismatches
			)",
			                       target_relation, expected_relation, expected_relation, target_relation);
			auto result = connection->Query(mismatch_query);
			if (result->HasError()) {
				result->ThrowError(StringUtil::Format("Failed to compare converted %s history: ", name));
			}
			auto chunk = result->Fetch();
			if (!chunk || chunk->GetValue(0, 0).GetValue<int64_t>() != 0) {
				throw InvalidInputException(
				    "Iceberg history is not an append-only continuation of the seeded DuckLake catalog (%s differs); "
				    "reseeding is required",
				    name);
			}
		};

		verify_relation(
		    "snapshots", StringUtil::Format("SELECT * FROM %s.ducklake_snapshot", target),
		    StringUtil::Format("SELECT * FROM %s.ducklake_snapshot WHERE snapshot_id <= %d", expected, target_max));
		verify_relation("snapshot changes", StringUtil::Format("SELECT * FROM %s.ducklake_snapshot_changes", target),
		                StringUtil::Format("SELECT * FROM %s.ducklake_snapshot_changes WHERE snapshot_id <= %d",
		                                   expected, target_max));
		verify_relation("schema versions", StringUtil::Format("SELECT * FROM %s.ducklake_schema_versions", target),
		                StringUtil::Format("SELECT * FROM %s.ducklake_schema_versions WHERE begin_snapshot <= %d",
		                                   expected, target_max));

		for (auto &table : {"ducklake_schema", "ducklake_table", "ducklake_column", "ducklake_partition_info",
		                    "ducklake_data_file", "ducklake_delete_file"}) {
			verify_relation(
			    table, StringUtil::Format("SELECT * FROM %s.%s", target, table),
			    StringUtil::Format("SELECT * REPLACE (CASE WHEN end_snapshot > %d THEN NULL ELSE end_snapshot END AS "
			                       "end_snapshot) FROM %s.%s WHERE begin_snapshot <= %d",
			                       target_max, expected, table, target_max));
		}

		verify_relation(
		    "partition columns", StringUtil::Format("SELECT * FROM %s.ducklake_partition_column", target),
		    StringUtil::Format("SELECT child.* FROM %s.ducklake_partition_column child JOIN %s.ducklake_partition_info "
		                       "parent USING(partition_id, table_id) WHERE parent.begin_snapshot <= %d",
		                       expected, expected, target_max));
		for (auto &table : {"ducklake_file_column_stats", "ducklake_file_partition_value"}) {
			verify_relation(table, StringUtil::Format("SELECT * FROM %s.%s", target, table),
			                StringUtil::Format("SELECT child.* FROM %s.%s child JOIN %s.ducklake_data_file parent "
			                                   "USING(data_file_id, table_id) WHERE parent.begin_snapshot <= %d",
			                                   expected, table, expected, target_max));
		}

		if (expected_max > target_max) {
			auto ordering_query = StringUtil::Format(
			    "SELECT min(expected.snapshot_time) > max(target.snapshot_time) FROM %s.ducklake_snapshot expected, "
			    "%s.ducklake_snapshot target WHERE expected.snapshot_id > %d",
			    expected, target, target_max);
			auto ordering_result = connection->Query(ordering_query);
			if (ordering_result->HasError()) {
				ordering_result->ThrowError("Failed to validate incremental snapshot ordering: ");
			}
			auto ordering_chunk = ordering_result->Fetch();
			if (!ordering_chunk || ordering_chunk->GetValue(0, 0).IsNull() ||
			    !ordering_chunk->GetValue(0, 0).GetValue<bool>()) {
				throw InvalidInputException(
				    "A newly observed Iceberg event is not newer than the DuckLake watermark; reseeding is required");
			}
		}
	}

	string OwnershipInsertSQL(const iceberg::ducklake::IcebergToDuckLakeBindData &bind_data,
	                          const string &fingerprint) {
		auto target = SQLIdentifier::ToString(metadata_catalog);
		return StringUtil::Format(R"(
			DELETE FROM %s.ducklake_metadata WHERE key IN (%s, %s, %s, %s);
			INSERT INTO %s.ducklake_metadata VALUES
				(%s, '1', NULL, NULL),
				(%s, %s, NULL, NULL),
				(%s, %s, NULL, NULL),
				(%s, %s, NULL, NULL);
		)",
		                          target, SQLString::ToString(OWNER_VERSION_KEY), SQLString::ToString(OWNER_SOURCE_KEY),
		                          SQLString::ToString(OWNER_SCOPE_KEY), SQLString::ToString(OWNER_FINGERPRINT_KEY),
		                          target, SQLString::ToString(OWNER_VERSION_KEY), SQLString::ToString(OWNER_SOURCE_KEY),
		                          SQLString::ToString(bind_data.source_identity), SQLString::ToString(OWNER_SCOPE_KEY),
		                          SQLString::ToString(bind_data.scope_identity),
		                          SQLString::ToString(OWNER_FINGERPRINT_KEY), SQLString::ToString(fingerprint));
	}

	idx_t ApplySeed(const iceberg::ducklake::IcebergToDuckLakeBindData &bind_data) {
		MaterializeExpectedCatalog(bind_data);
		auto fingerprint = ComputeCatalogFingerprint(EXPECTED_CATALOG);
		auto statements = bind_data.sql_statements;
		D_ASSERT(!statements.empty() && statements.back() == "COMMIT TRANSACTION;");
		statements.insert(statements.end() - 1, OwnershipInsertSQL(bind_data, fingerprint));
		auto query = StringUtil::Join(statements, "\n");
		query = StringUtil::Replace(query, "{METADATA_CATALOG}", SQLIdentifier::ToString(metadata_catalog));
		ExecuteOrThrow(query, "'iceberg_to_ducklake' failed to seed the DuckLake metadata catalog: ");
		return bind_data.tables.size();
	}

	idx_t ApplyIncremental(const iceberg::ducklake::IcebergToDuckLakeBindData &bind_data) {
		MaterializeExpectedCatalog(bind_data);
		VerifyExpectedPrefix();
		auto target_max = GetMaxSnapshot(metadata_catalog);
		auto expected_max = GetMaxSnapshot(EXPECTED_CATALOG);
		if (target_max == expected_max) {
			if (ComputeCatalogFingerprint(metadata_catalog) != ComputeCatalogFingerprint(EXPECTED_CATALOG)) {
				throw InvalidInputException(
				    "Iceberg metadata changed without an append-only event; reseeding is required");
			}
			return 0;
		}

		auto target = SQLIdentifier::ToString(metadata_catalog);
		auto expected = SQLIdentifier::ToString(EXPECTED_CATALOG);
		auto fingerprint = ComputeCatalogFingerprint(EXPECTED_CATALOG);
		vector<string> statements;
		statements.push_back("BEGIN TRANSACTION;");
		for (auto &entry : {pair<const char *, const char *>("ducklake_snapshot", "snapshot_id"),
		                    pair<const char *, const char *>("ducklake_snapshot_changes", "snapshot_id"),
		                    pair<const char *, const char *>("ducklake_schema_versions", "begin_snapshot")}) {
			statements.push_back(StringUtil::Format("INSERT INTO %s.%s SELECT * FROM %s.%s WHERE %s > %d;", target,
			                                        entry.first, expected, entry.first, entry.second, target_max));
		}

		statements.push_back(StringUtil::Format(
		    "UPDATE %s.ducklake_column AS target_row SET end_snapshot = expected_row.end_snapshot FROM "
		    "%s.ducklake_column AS expected_row WHERE target_row.table_id = expected_row.table_id AND "
		    "target_row.column_id = expected_row.column_id AND target_row.begin_snapshot = expected_row.begin_snapshot "
		    "AND target_row.end_snapshot IS NULL AND expected_row.end_snapshot > %d;",
		    target, expected, target_max));
		statements.push_back(StringUtil::Format(
		    "UPDATE %s.ducklake_partition_info AS target_row SET end_snapshot = expected_row.end_snapshot FROM "
		    "%s.ducklake_partition_info AS expected_row WHERE target_row.table_id = expected_row.table_id AND "
		    "target_row.partition_id = expected_row.partition_id AND "
		    "target_row.begin_snapshot = expected_row.begin_snapshot AND target_row.end_snapshot IS NULL AND "
		    "expected_row.end_snapshot > %d;",
		    target, expected, target_max));
		for (auto &entry : {pair<const char *, const char *>("ducklake_data_file", "data_file_id"),
		                    pair<const char *, const char *>("ducklake_delete_file", "delete_file_id")}) {
			statements.push_back(StringUtil::Format(
			    "UPDATE %s.%s AS target_row SET end_snapshot = expected_row.end_snapshot FROM %s.%s AS expected_row "
			    "WHERE target_row.%s = expected_row.%s AND target_row.end_snapshot IS NULL AND "
			    "expected_row.end_snapshot > %d;",
			    target, entry.first, expected, entry.first, entry.second, entry.second, target_max));
		}

		for (auto &table : {"ducklake_schema", "ducklake_table", "ducklake_column", "ducklake_partition_info",
		                    "ducklake_data_file", "ducklake_delete_file"}) {
			statements.push_back(StringUtil::Format("INSERT INTO %s.%s SELECT * FROM %s.%s WHERE begin_snapshot > %d;",
			                                        target, table, expected, table, target_max));
		}
		statements.push_back(StringUtil::Format(
		    "INSERT INTO %s.ducklake_partition_column SELECT child.* FROM %s.ducklake_partition_column child JOIN "
		    "%s.ducklake_partition_info parent USING(partition_id, table_id) WHERE parent.begin_snapshot > %d;",
		    target, expected, expected, target_max));
		for (auto &table : {"ducklake_file_column_stats", "ducklake_file_partition_value"}) {
			statements.push_back(StringUtil::Format(
			    "INSERT INTO %s.%s SELECT child.* FROM %s.%s child JOIN %s.ducklake_data_file parent "
			    "USING(data_file_id, table_id) WHERE parent.begin_snapshot > %d;",
			    target, table, expected, table, expected, target_max));
		}

		for (auto &table : {"ducklake_table_stats", "ducklake_table_column_stats"}) {
			statements.push_back(StringUtil::Format("DELETE FROM %s.%s;", target, table));
			statements.push_back(
			    StringUtil::Format("INSERT INTO %s.%s SELECT * FROM %s.%s;", target, table, expected, table));
		}
		statements.push_back(OwnershipInsertSQL(bind_data, fingerprint));
		statements.push_back("COMMIT TRANSACTION;");
		auto query = StringUtil::Join(statements, "\n");
		ExecuteOrThrow(query, "'iceberg_to_ducklake' failed to append incremental metadata: ");
		return bind_data.tables.size();
	}

public:
	static unique_ptr<GlobalTableFunctionState> Init(ClientContext &context, TableFunctionInitInput &input) {
		auto &bind_data = input.bind_data->Cast<iceberg::ducklake::IcebergToDuckLakeBindData>();
		auto &input_string = bind_data.ducklake_catalog;

		auto &catalog = Catalog::GetCatalog(context, Identifier(input_string));
		auto catalog_type = catalog.GetCatalogType();
		if (catalog_type != "ducklake") {
			throw InvalidInputException("Second parameter must be the name of an attached DuckLake catalog");
		}

		auto metadata_catalog = StringUtil::Format("__ducklake_metadata_%s", input_string);
		//! Verify the existence of the metadata catalog and that it's attached as well.
		(void)Catalog::GetCatalog(context, Identifier(metadata_catalog));

		auto &db = DatabaseInstance::GetDatabase(context);
		auto connection = make_uniq<Connection>(db);
		auto res = make_uniq<IcebergToDuckLakeGlobalTableFunctionState>(std::move(connection), metadata_catalog);
		res->VerifyDuckLakeVersion();
		res->LoadOwnership(bind_data);
		return std::move(res);
	}

public:
	//! Connection used to run the SQL statements
	unique_ptr<Connection> connection;
	string metadata_catalog;
	bool incremental = false;
	bool finished = false;
};

static void IcebergToDuckLakeFunction(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
	auto &bind_data = data.bind_data->Cast<iceberg::ducklake::IcebergToDuckLakeBindData>();
	auto &global_state = data.global_state->Cast<IcebergToDuckLakeGlobalTableFunctionState>();
	if (global_state.finished) {
		output.SetChildCardinality(0);
		return;
	}
	auto converted_count =
	    global_state.incremental ? global_state.ApplyIncremental(bind_data) : global_state.ApplySeed(bind_data);
	output.data[0].SetValue(0, Value::BIGINT(NumericCast<int64_t>(converted_count)));
	output.SetChildCardinality(1);
	global_state.finished = true;
}

TableFunctionSet IcebergFunctions::GetIcebergToDuckLakeFunction() {
	TableFunctionSet function_set("iceberg_to_ducklake");

	auto fun = TableFunction({LogicalType::VARCHAR, LogicalType::VARCHAR}, IcebergToDuckLakeFunction,
	                         iceberg::ducklake::IcebergToDuckLakeBind, IcebergToDuckLakeGlobalTableFunctionState::Init);
	fun.named_parameters.emplace("skip_tables", LogicalType::LIST(LogicalTypeId::VARCHAR));
	fun.named_parameters.emplace("schema", LogicalType::VARCHAR);
	fun.named_parameters.emplace("table", LogicalType::VARCHAR);
	function_set.AddFunction(fun);

	return function_set;
}

} // namespace duckdb
