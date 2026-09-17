#pragma once

#include "duckdb/common/multi_file/multi_file_reader.hpp"
#include "planning/deletes/iceberg_equality_delete_fast_filter.hpp"
#include "planning/iceberg_multi_file_list.hpp"

namespace duckdb {

struct IcebergEqualityDeleteReadColumn {
	int32_t field_id;
	idx_t expression_index;
	LogicalType type;
};

struct IcebergEqualityDeleteReadState {
	explicit IcebergEqualityDeleteReadState(vector<IcebergEqualityDeleteReadColumn> columns_p)
	    : columns(std::move(columns_p)) {
		for (idx_t i = 0; i < columns.size(); i++) {
			field_indexes.emplace(columns[i].field_id, i);
			types.push_back(columns[i].type);
		}
	}

	vector<IcebergEqualityDeleteReadColumn> columns;
	vector<LogicalType> types;
	unordered_map<int32_t, idx_t> field_indexes;
	unique_ptr<Expression> expression;
	shared_ptr<const IcebergEqualityDeleteFastFilter> fast_filter;
};

struct IcebergScanReaderGlobalState : public MultiFileReaderGlobalState {
public:
	explicit IcebergScanReaderGlobalState(const MultiFileList &file_list_p)
	    : MultiFileReaderGlobalState({}, file_list_p, true) {
	}

	void CacheEqualityDeleteReadState(idx_t file_list_idx, unique_ptr<IcebergEqualityDeleteReadState> read_state) {
		lock_guard<mutex> guard(equality_delete_read_state_lock);
		auto &cached_state = equality_delete_read_states[file_list_idx];
		if (cached_state) {
			throw InternalException("Equality-delete state was initialized twice for file-list index %llu",
			                        file_list_idx);
		}
		cached_state = std::move(read_state);
	}

	const IcebergEqualityDeleteReadState &GetEqualityDeleteReadState(idx_t file_list_idx) const {
		lock_guard<mutex> guard(equality_delete_read_state_lock);
		auto entry = equality_delete_read_states.find(file_list_idx);
		if (entry == equality_delete_read_states.end()) {
			throw InternalException("Equality-delete state was not initialized for file-list index %llu",
			                        file_list_idx);
		}
		return *entry->second;
	}

	IcebergEqualityDeleteFastFilter::BuildResult
	GetOrCreateEqualityDeleteFastFilter(const vector<reference<const IcebergEqualityDeleteFile>> &delete_files,
	                                    const IcebergEqualityDeleteReadState &read_state,
	                                    const set<int32_t> &local_field_ids, ClientContext &context,
	                                    Allocator &allocator);

private:
	mutable mutex equality_delete_read_state_lock;
	//! The values are heap allocated so references remain stable while other files are initialized in parallel.
	unordered_map<idx_t, unique_ptr<IcebergEqualityDeleteReadState>> equality_delete_read_states;
	IcebergEqualityDeleteFastFilterCache equality_delete_fast_filter_cache;
};

//! File execution shared by metadata-planned and materialized-task scans.
//! Owns no planner or task source; callers supply schemas, deletes and partition constants.
struct IcebergScanReader {
	static constexpr column_t COLUMN_IDENTIFIER_LAST_SEQUENCE_NUMBER = UINT64_C(10000000000000000000);

	IcebergScanReader();
	ReaderInitializeType InitializeReader(MultiFileReader &multi_file_reader, MultiFileReaderData &reader_data,
	                                      const MultiFileBindData &bind_data,
	                                      const vector<MultiFileColumnDefinition> &global_columns,
	                                      const vector<ColumnIndex> &global_column_ids,
	                                      optional_ptr<TableFilterSet> table_filters, ClientContext &context,
	                                      MultiFileGlobalState &gstate, const IcebergTableMetadataSchemas &schemas,
	                                      const vector<IcebergFieldMapping> &mappings, IcebergDeletePlan delete_plan,
	                                      const unordered_map<int32_t, Value> &partition_constants);

	void BindOptions(MultiFileReader &multi_file_reader, MultiFileOptions &options, MultiFileList &files,
	                 vector<LogicalType> &return_types, vector<Identifier> &names, MultiFileReaderBindData &bind_data);
	void FinalizeChunk(MultiFileReader &multi_file_reader, ClientContext &context, const MultiFileBindData &bind_data,
	                   BaseFileReader &reader, const MultiFileReaderData &reader_data, DataChunk &input_chunk,
	                   DataChunk &output_chunk, ExpressionExecutor &executor,
	                   optional_ptr<MultiFileReaderGlobalState> global_state);
	MultiFileReaderVirtualColumnBinding
	GetVirtualColumnExpression(MultiFileReader &multi_file_reader, ClientContext &context,
	                           MultiFileReaderData &reader_data, const vector<MultiFileColumnDefinition> &local_columns,
	                           const idx_t column_id, const LogicalType &type, MultiFileLocalIndex local_idx);

private:
	static unique_ptr<Expression>
	CreateEqualityDeleteExpression(const vector<reference<const IcebergEqualityDeleteFile>> &delete_files,
	                               const vector<MultiFileColumnDefinition> &local_columns,
	                               const IcebergEqualityDeleteReadState &read_state,
	                               const vector<bool> &accelerated_files);
	static vector<IcebergEqualityDeleteReadColumn>
	AddEqualityDeleteColumns(const IcebergTableMetadataSchemas &schemas,
	                         const vector<reference<const IcebergEqualityDeleteFile>> &delete_files,
	                         vector<MultiFileColumnDefinition> &scan_columns, vector<ColumnIndex> &scan_column_ids,
	                         MultiFileReaderData &reader_data, ClientContext &context);
	static IcebergEqualityDeleteReadColumn
	AddEqualityDeleteColumn(const IcebergTableMetadataSchemas &schemas, int32_t field_id,
	                        vector<MultiFileColumnDefinition> &scan_columns, vector<ColumnIndex> &scan_column_ids,
	                        MultiFileReaderData &reader_data, ClientContext &context);
	static void ApplyPartitionConstants(const unordered_map<int32_t, Value> &constants,
	                                    MultiFileReaderData &reader_data,
	                                    const vector<MultiFileColumnDefinition> &global_columns,
	                                    const vector<ColumnIndex> &global_column_ids);

	unique_ptr<MultiFileColumnDefinition> row_id_column;
	unique_ptr<MultiFileColumnDefinition> last_updated_sequence_number_column;
};

} // namespace duckdb
