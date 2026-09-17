#include "planning/iceberg_multi_file_reader.hpp"

#include "duckdb/catalog/catalog_entry/table_function_catalog_entry.hpp"
#include "duckdb/function/partition_stats.hpp"
#include "duckdb/logging/logger.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/function/function_binder.hpp"
#include "duckdb/execution/execution_context.hpp"
#include "duckdb/parallel/thread_context.hpp"
#include "duckdb/parser/tableref/table_function_ref.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/planner/expression/bound_comparison_expression.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/planner/expression/bound_operator_expression.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"
#include "duckdb/planner/expression/bound_conjunction_expression.hpp"

#include "common/iceberg_utils.hpp"
#include "iceberg_logging.hpp"
#include "planning/iceberg_multi_file_list.hpp"
#include "planning/pruning/iceberg_predicate.hpp"
#include "core/expression/iceberg_value.hpp"
#include "core/expression/iceberg_predicate_stats.hpp"
#include "core/metadata/iceberg_table_metadata.hpp"
#include "duckdb/common/multi_file/multi_file_states.hpp"

#include <cstdint>

namespace duckdb {

IcebergMultiFileReader::IcebergMultiFileReader(shared_ptr<TableFunctionInfo> function_info)
    : function_info(std::move(function_info)) {
}

unique_ptr<MultiFileReader> IcebergMultiFileReader::CreateInstance(const TableFunction &table) {
	return make_uniq<IcebergMultiFileReader>(table.function_info);
}

shared_ptr<MultiFileList> IcebergMultiFileReader::CreateFileList(ClientContext &context, const vector<string> &paths,
                                                                 const FileGlobInput &glob_input) {
	if (paths.size() != 1) {
		throw BinderException("'iceberg_scan' only supports single path as input");
	}

	//! Scan initiated from a REST Catalog
	auto scan_info = shared_ptr_cast<TableFunctionInfo, IcebergScanInfo>(function_info);
	return make_shared_ptr<IcebergMultiFileList>(context, scan_info, paths[0], options);
}

bool IcebergMultiFileReader::Bind(MultiFileOptions &options, MultiFileList &files, vector<LogicalType> &return_types,
                                  vector<Identifier> &names, MultiFileReaderBindData &bind_data) {
	auto &iceberg_multi_file_list = dynamic_cast<IcebergMultiFileList &>(files);

	iceberg_multi_file_list.GetScanPlanner().SetOptions(this->options);
	iceberg_multi_file_list.Bind(return_types, names);
	// FIXME: apply final transformation for 'file_row_number' ???
	auto &schema = iceberg_multi_file_list.GetScanPlanner().GetSchema().columns;
	auto &columns = bind_data.schema;
	for (auto &item : schema) {
		columns.push_back(item->GetMultiFileColumnDefinition());
	}

	bind_data.mapping = MultiFileColumnMappingMode::BY_FIELD_ID;
	return true;
}

void IcebergMultiFileReader::BindOptions(MultiFileOptions &options, MultiFileList &files,
                                         vector<LogicalType> &return_types, vector<Identifier> &names,
                                         MultiFileReaderBindData &bind_data) {
	scan_reader.BindOptions(*this, options, files, return_types, names, bind_data);
}

unique_ptr<MultiFileReaderGlobalState>
IcebergMultiFileReader::InitializeGlobalState(ClientContext &context, const MultiFileOptions &file_options,
                                              const MultiFileReaderBindData &bind_data, const MultiFileList &file_list,
                                              const vector<MultiFileColumnDefinition> &global_columns,
                                              const vector<ColumnIndex> &global_column_ids) {
	return make_uniq<IcebergScanReaderGlobalState>(file_list);
}

unordered_map<int32_t, Value> IcebergMultiFileReader::PartitionConstants(
    int32_t partition_spec_id, const BoundIcebergManifestEntry &bound_manifest_entry,
    const unordered_map<int32_t, IcebergPartitionSpec> &partition_specs, const IcebergTableMetadataSchemas &schemas,
    const vector<MultiFileColumnDefinition> &global_columns, ClientContext &context) {
	auto spec = partition_specs.find(partition_spec_id);
	if (spec == partition_specs.end()) {
		throw InvalidConfigurationException("'partition_spec_id' %d doesn't exist in the metadata", partition_spec_id);
	}
	unordered_map<int32_t, idx_t> field_indexes;
	for (idx_t i = 0; i < spec->second.fields.size(); i++) {
		field_indexes[spec->second.fields[i].source_id] = i;
	}
	unordered_map<int32_t, Value> constants;
	for (auto &item : field_indexes) {
		auto &field = spec->second.fields[item.second];
		if (field.transform != IcebergTransformType::IDENTITY) {
			continue;
		}
		optional_ptr<const LogicalType> type;
		for (auto &column : global_columns) {
			if (!column.identifier.IsNull() && column.GetIdentifierFieldId() == item.first) {
				type = column.type;
				break;
			}
		}
		if (!type) {
			auto column = schemas.FindColumnByFieldId(item.first);
			if (column) {
				type = column->type;
			}
		}
		if (!type) {
			continue;
		}
		for (auto &partition : bound_manifest_entry.entry.data_file.partition_info) {
			if (partition.field_id == field.partition_field_id && !partition.value.IsNull()) {
				constants.emplace(item.first, IcebergValue::TransformPartitionValue(partition.value, *type));
				break;
			}
		}
	}
	return constants;
}

ReaderInitializeType IcebergMultiFileReader::InitializeReader(MultiFileReaderData &reader_data,
                                                              const MultiFileBindData &bind_data,
                                                              const vector<MultiFileColumnDefinition> &global_columns,
                                                              const vector<ColumnIndex> &global_column_ids,
                                                              optional_ptr<TableFilterSet> table_filters,
                                                              ClientContext &context, MultiFileGlobalState &gstate) {
	auto &iceberg_state = gstate.multi_file_reader_state->Cast<IcebergScanReaderGlobalState>();
	const auto &multi_file_list = dynamic_cast<const IcebergMultiFileList &>(*iceberg_state.file_list);
	auto &planner = multi_file_list.GetScanPlanner();
	auto &metadata = planner.GetMetadata();
	auto file_id = reader_data.reader->file_list_idx.GetIndex();
	auto task = planner.GetScanTask(file_id);
	if (!task) {
		throw InternalException("Unable to find Iceberg scan task for file index %llu", file_id);
	}
	int32_t partition_spec_id;
	planner.WithManifestFile(
	    task->manifest_entry, IcebergManifestContentType::DATA,
	    [&](const IcebergManifestFile &manifest) { partition_spec_id = manifest.partition_spec_id; });
	auto constants = PartitionConstants(partition_spec_id, task->manifest_entry, metadata.partition_specs,
	                                    metadata.GetSchemas(), global_columns, context);
	return scan_reader.InitializeReader(*this, reader_data, bind_data, global_columns, global_column_ids, table_filters,
	                                    context, gstate, metadata.GetSchemas(), metadata.mappings,
	                                    multi_file_list.ProcessDeletes(*task), constants);
}

void IcebergMultiFileReader::FinalizeBind(MultiFileReaderData &reader_data, const MultiFileOptions &file_options,
                                          const MultiFileReaderBindData &options,
                                          const vector<MultiFileColumnDefinition> &global_columns,
                                          const vector<ColumnIndex> &global_column_ids, ClientContext &context,
                                          optional_ptr<MultiFileReaderGlobalState> global_state) {
	throw InternalException("IcebergMultiFileReader::FinalizeBind is unreachable");
}

bool IcebergMultiFileReader::ParseOption(const Identifier &key, const Value &val, MultiFileOptions &options,
                                         ClientContext &context) {
	auto &snapshot_lookup = this->options.snapshot_lookup;

	if (key == "allow_moved_paths") {
		this->options.allow_moved_paths = BooleanValue::Get(val);
		return true;
	}
	if (key == "metadata_compression_codec") {
		this->options.metadata_compression_codec = StringValue::Get(val);
		return true;
	}
	if (key == "version") {
		this->options.table_version = StringValue::Get(val);
		this->options.version_explicitly_set = true;
		return true;
	}
	if (key == "version_name_format") {
		auto value = StringValue::Get(val);
		auto string_substitutions = IcebergUtils::CountOccurrences(value, "%s");
		if (string_substitutions != 2) {
			throw InvalidInputException("'version_name_format' has to contain two occurrences of '%%s' in it, found %d",
			                            string_substitutions);
		}
		this->options.version_name_format = value;
		return true;
	}
	if (key == "snapshot_from_id") {
		if (snapshot_lookup->GetSource() != SnapshotSource::LATEST) {
			throw InvalidInputException("Can't use 'snapshot_from_id' in combination with 'snapshot_from_timestamp'");
		}
		snapshot_lookup.emplace(IcebergSnapshotLookup::FromSnapshotId(val.GetValue<uint64_t>()));
		return true;
	}
	if (key == "snapshot_from_timestamp") {
		if (snapshot_lookup->GetSource() != SnapshotSource::LATEST) {
			throw InvalidInputException("Can't use 'snapshot_from_id' in combination with 'snapshot_from_timestamp'");
		}
		snapshot_lookup.emplace(IcebergSnapshotLookup::FromTimestamp(
		    val.DefaultCastAs(LogicalType::TIMESTAMP_MS).GetValue<timestamp_ms_t>()));
		return true;
	}
	return MultiFileReader::ParseOption(key, val, options, context);
}

vector<PartitionStatistics> IcebergMultiFileReader::IcebergGetPartitionStats(ClientContext &context,
                                                                             GetPartitionStatsInput &input) {
	auto &bind_data = input.bind_data->Cast<MultiFileBindData>();
	vector<PartitionStatistics> result;
	auto &multi_file_list = bind_data.file_list->Cast<IcebergMultiFileList>();
	auto &scan_planner = multi_file_list.GetScanPlanner();
	scan_planner.GetStatistics(result);
	return result;
}

void IcebergMultiFileReader::FinalizeChunk(ClientContext &context, const MultiFileBindData &bind_data,
                                           BaseFileReader &reader, const MultiFileReaderData &reader_data,
                                           DataChunk &input_chunk, DataChunk &output_chunk,
                                           ExpressionExecutor &executor,
                                           optional_ptr<MultiFileReaderGlobalState> global_state) {
	scan_reader.FinalizeChunk(*this, context, bind_data, reader, reader_data, input_chunk, output_chunk, executor,
	                          global_state);
}

MultiFileReaderVirtualColumnBinding IcebergMultiFileReader::GetVirtualColumnExpression(
    ClientContext &context, MultiFileReaderData &reader_data, const vector<MultiFileColumnDefinition> &local_columns,
    const idx_t column_id, const LogicalType &type, MultiFileLocalIndex local_idx) {
	return scan_reader.GetVirtualColumnExpression(*this, context, reader_data, local_columns, column_id, type,
	                                              local_idx);
}

} // namespace duckdb
