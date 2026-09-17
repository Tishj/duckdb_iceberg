//===----------------------------------------------------------------------===//
//                         DuckDB
//
// iceberg_multi_file_reader.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "planning/iceberg_scan_reader.hpp"

#include "duckdb/common/multi_file/multi_file_reader.hpp"
#include "duckdb/common/types/batched_data_collection.hpp"
#include "duckdb/common/multi_file/multi_file_data.hpp"
#include "duckdb/common/list.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/common/unordered_map.hpp"
#include "duckdb/planner/filter/constant_filter.hpp"
#include "duckdb/planner/filter/null_filter.hpp"
#include "duckdb/planner/table_filter.hpp"

#include "planning/iceberg_multi_file_list.hpp"
#include "planning/deletes/iceberg_equality_delete_fast_filter.hpp"
#include "common/iceberg_utils.hpp"
#include "planning/metadata_io/manifest/iceberg_manifest_reader.hpp"

namespace duckdb {

struct IcebergMultiFileReader : public MultiFileReader {
public:
	static constexpr column_t COLUMN_IDENTIFIER_LAST_SEQUENCE_NUMBER =
	    IcebergScanReader::COLUMN_IDENTIFIER_LAST_SEQUENCE_NUMBER;

public:
	IcebergMultiFileReader(shared_ptr<TableFunctionInfo> function_info);

public:
	static unique_ptr<MultiFileReader> CreateInstance(const TableFunction &table);
	static vector<PartitionStatistics> IcebergGetPartitionStats(ClientContext &context, GetPartitionStatsInput &input);

public:
	shared_ptr<MultiFileList> CreateFileList(ClientContext &context, const vector<string> &paths,
	                                         const FileGlobInput &glob_input) override;
	bool Bind(MultiFileOptions &options, MultiFileList &files, vector<LogicalType> &return_types,
	          vector<Identifier> &names, MultiFileReaderBindData &bind_data) override;
	void BindOptions(MultiFileOptions &options, MultiFileList &files, vector<LogicalType> &return_types,
	                 vector<Identifier> &names, MultiFileReaderBindData &bind_data) override;
	unique_ptr<MultiFileReaderGlobalState>
	InitializeGlobalState(ClientContext &context, const MultiFileOptions &file_options,
	                      const MultiFileReaderBindData &bind_data, const MultiFileList &file_list,
	                      const vector<MultiFileColumnDefinition> &global_columns,
	                      const vector<ColumnIndex> &global_column_ids) override;
	ReaderInitializeType InitializeReader(MultiFileReaderData &reader_data, const MultiFileBindData &bind_data,
	                                      const vector<MultiFileColumnDefinition> &global_columns,
	                                      const vector<ColumnIndex> &global_column_ids,
	                                      optional_ptr<TableFilterSet> table_filters, ClientContext &context,
	                                      MultiFileGlobalState &gstate) override;
	void FinalizeBind(MultiFileReaderData &reader_data, const MultiFileOptions &file_options,
	                  const MultiFileReaderBindData &options, const vector<MultiFileColumnDefinition> &global_columns,
	                  const vector<ColumnIndex> &global_column_ids, ClientContext &context,
	                  optional_ptr<MultiFileReaderGlobalState> global_state) override;
	void FinalizeChunk(ClientContext &context, const MultiFileBindData &bind_data, BaseFileReader &reader,
	                   const MultiFileReaderData &reader_data, DataChunk &input_chunk, DataChunk &output_chunk,
	                   ExpressionExecutor &executor, optional_ptr<MultiFileReaderGlobalState> global_state) override;
	bool ParseOption(const Identifier &key, const Value &val, MultiFileOptions &options,
	                 ClientContext &context) override;

	MultiFileReaderVirtualColumnBinding
	GetVirtualColumnExpression(ClientContext &context, MultiFileReaderData &reader_data,
	                           const vector<MultiFileColumnDefinition> &local_columns, const idx_t column_id,
	                           const LogicalType &type, MultiFileLocalIndex local_idx) override;

private:
	static unordered_map<int32_t, Value>
	PartitionConstants(int32_t partition_spec_id, const BoundIcebergManifestEntry &bound_manifest_entry,
	                   const unordered_map<int32_t, IcebergPartitionSpec> &partition_specs,
	                   const IcebergTableMetadataSchemas &schemas,
	                   const vector<MultiFileColumnDefinition> &global_columns, ClientContext &context);

public:
	shared_ptr<TableFunctionInfo> function_info;
	IcebergOptions options;

private:
	IcebergScanReader scan_reader;
};

} // namespace duckdb
