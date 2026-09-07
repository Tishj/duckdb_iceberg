#include "planning/iceberg_multi_file_list.hpp"

#include "duckdb/common/exception.hpp"
#include "duckdb/common/multi_file/multi_file_reader.hpp"
#include "duckdb/optimizer/filter_combiner.hpp"
#include "duckdb/planner/filter/expression_filter.hpp"
#include "duckdb/planner/table_filter_set.hpp"
#include "duckdb/storage/table/row_group_reorderer.hpp"

namespace duckdb {

IcebergMultiFileList::IcebergMultiFileList(ClientContext &context, shared_ptr<IcebergScanInfo> scan_info,
                                           const string &path, const IcebergOptions &options)
    : planner(make_uniq<IcebergScanPlanner>(context, std::move(scan_info), path, options)) {
}

IcebergMultiFileList::IcebergMultiFileList(unique_ptr<IcebergScanPlanner> planner_p) : planner(std::move(planner_p)) {
}

IcebergMultiFileList::~IcebergMultiFileList() {
}

IcebergScanPlanner &IcebergMultiFileList::GetScanPlanner() {
	return *planner;
}

const IcebergScanPlanner &IcebergMultiFileList::GetScanPlanner() const {
	return *planner;
}

void IcebergMultiFileList::SetTable(IcebergTableSchemaVersion &table) {
	planner->SetTable(table);
}

optional_ptr<IcebergTableSchemaVersion> IcebergMultiFileList::GetTable() const {
	return planner->GetTable();
}

void IcebergMultiFileList::SetOptions(const IcebergOptions &options) {
	planner->SetOptions(options);
}

void IcebergMultiFileList::SetScanOrder(unique_ptr<RowGroupOrderOptions> options) {
	planner->SetScanOrder(std::move(options));
}

void IcebergMultiFileList::DisableServerSidePlanning() {
	planner->DisableServerSidePlanning();
}

void IcebergMultiFileList::Bind(vector<LogicalType> &return_types, vector<Identifier> &names) {
	planner->Bind(return_types, names);
}

const IcebergTableMetadata &IcebergMultiFileList::GetMetadata() const {
	return planner->GetMetadata();
}

const IcebergTableSchema &IcebergMultiFileList::GetSchema() const {
	return planner->GetSchema();
}

IcebergPartition IcebergMultiFileList::GetPartitionForDataFile(const string &file_path) const {
	return planner->GetPartitionForDataFile(file_path);
}

shared_ptr<IcebergDeleteData> IcebergMultiFileList::GetExistingPositionalDeleteData(const string &file_path) const {
	return planner->GetExistingPositionalDeleteData(file_path);
}

void IcebergMultiFileList::GetStatistics(vector<PartitionStatistics> &result) const {
	planner->GetStatistics(result);
}

unique_ptr<IcebergMultiFileList>
IcebergMultiFileList::PushdownInternal(TableFilterSet &new_filters, const vector<ColumnIndex> &column_indexes) const {
	IcebergTableFilters result_filter_set;
	for (auto &entry : new_filters) {
		auto projection_index = ProjectionIndex(entry.GetIndex().GetIndex());
		auto &column_index = column_indexes[projection_index];
		auto primary_index = column_index.GetPrimaryIndex();
		if (primary_index >= planner->Names().size()) {
			continue;
		}
		auto &filter = ExpressionFilter::GetExpressionFilter(entry.Filter(), "IcebergMultiFileList::PushdownInternal");
		result_filter_set.PushFilter(column_index, filter.Copy());
	}
	return unique_ptr<IcebergMultiFileList>(
	    new IcebergMultiFileList(planner->CreateView(std::move(result_filter_set))));
}

unique_ptr<MultiFileList>
IcebergMultiFileList::DynamicFilterPushdown(MultiFileDynamicPushdownInfo &pushdown_info) const {
	auto &column_indexes = pushdown_info.column_indexes;
	auto &filters = pushdown_info.filters;
	if (!filters.HasFilters()) {
		return nullptr;
	}

	auto filters_copy = filters.Copy();
	D_ASSERT(filters_copy->FilterCount() >= planner->Filters().FilterCount());
	bool filters_changed = false;
	for (auto &entry : filters) {
		auto &filter =
		    ExpressionFilter::GetExpressionFilter(entry.Filter(), "IcebergMultiFileList::DynamicFilterPushdown");
		auto column_id = column_indexes[entry.GetIndex().GetIndex()];
		auto previous = planner->Filters().TryGetFilterByColumnIndex(column_id);
		if (!previous || !filter.Equals(*previous)) {
			filters_changed = true;
		}
	}
	return filters_changed ? PushdownInternal(*filters_copy, column_indexes) : nullptr;
}

unique_ptr<MultiFileList> IcebergMultiFileList::ComplexFilterPushdown(ClientContext &context, const MultiFileOptions &,
                                                                      MultiFilePushdownInfo &info,
                                                                      vector<unique_ptr<Expression>> &filters) const {
	if (filters.empty()) {
		return nullptr;
	}
	FilterCombiner combiner(context);
	for (const auto &filter : filters) {
		combiner.AddFilter(filter->Copy());
	}
	vector<FilterPushdownResult> unused;
	auto filter_set = combiner.GenerateTableScanFilters(info.column_indexes, unused);
	if (!filter_set.HasFilters()) {
		return nullptr;
	}
	return PushdownInternal(filter_set, info.column_indexes);
}

OpenFileInfo IcebergMultiFileList::GetFileInternal(idx_t file_id) const {
	auto task = planner->GetDataFileTask(file_id);
	if (!task) {
		return OpenFileInfo();
	}
	auto &data_file = task->data_file.entry.data_file;
	if (!StringUtil::CIEquals(data_file.file_format, "parquet")) {
		throw NotImplementedException("File format '%s' not supported, only supports 'parquet' currently",
		                              data_file.file_format);
	}
	OpenFileInfo result(task->file_path);
	auto extended_info = make_shared_ptr<ExtendedOpenFileInfo>();
	extended_info->options["file_size"] = Value::UBIGINT(data_file.file_size_in_bytes);
	extended_info->options["validate_external_file_cache"] = Value::BOOLEAN(false);
	extended_info->options["etag"] = Value("");
	extended_info->options["last_modified"] = Value::TIMESTAMP(timestamp_t(0));
	if (task->data_file.HasFirstRowId()) {
		extended_info->options["first_row_id"] = Value::BIGINT(task->data_file.GetFirstRowId());
	}
	extended_info->options["sequence_number"] =
	    Value::BIGINT(task->data_file.entry.GetSequenceNumber(task->manifest_file));
	result.extended_info = std::move(extended_info);
	return result;
}

vector<OpenFileInfo> IcebergMultiFileList::GetAllFiles() const {
	vector<OpenFileInfo> result;
	for (idx_t i = 0;; i++) {
		auto file = GetFileInternal(i);
		if (file.path.empty()) {
			break;
		}
		result.push_back(std::move(file));
	}
	return result;
}

FileExpandResult IcebergMultiFileList::GetExpandResult() const {
	if (planner->IsBound()) {
		GetFileInternal(1);
	}
	return FileExpandResult::MULTIPLE_FILES;
}

idx_t IcebergMultiFileList::GetTotalFileCount() const {
	return planner->GetTotalFileCount();
}

unique_ptr<NodeStatistics> IcebergMultiFileList::GetCardinality(ClientContext &) const {
	return planner->GetCardinality();
}

OpenFileInfo IcebergMultiFileList::GetFile(idx_t file_id) const {
	return GetFileInternal(file_id);
}

} // namespace duckdb
