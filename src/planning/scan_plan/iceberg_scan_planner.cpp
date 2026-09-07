#include "planning/scan_plan/iceberg_scan_planner.hpp"

#include "catalog/rest/catalog_entry/table/iceberg_table_schema_version.hpp"
#include "common/iceberg_utils.hpp"
#include "core/metadata/iceberg_table_metadata.hpp"
#include "planning/deletes/iceberg_delete_file_scanner.hpp"
#include "planning/pruning/iceberg_file_pruner.hpp"
#include "planning/scan_plan/iceberg_scan_plan_provider.hpp"
#include "duckdb/storage/table/row_group_reorderer.hpp"

namespace duckdb {

namespace {

static void MergeDeleteScanResult(IcebergScanPlanProvider &provider, IcebergDeleteScanResult &&scan_result) {
	auto &positional_delete_data = provider.PositionalDeleteData();
	for (auto &entry : scan_result.positional_delete_data) {
		auto existing = positional_delete_data.find(entry.first);
		if (existing == positional_delete_data.end()) {
			positional_delete_data.emplace(entry.first, std::move(entry.second));
			continue;
		}
		auto &target = existing->second;
		auto &source = entry.second;
		if (target->type == IcebergDeleteType::DELETION_VECTOR) {
			if (source->type == IcebergDeleteType::DELETION_VECTOR) {
				throw InvalidConfigurationException(
				    "Table is corrupt, two or more deletion vectors exist for the same referenced_data_file");
			}
			continue;
		}
		if (source->type == IcebergDeleteType::DELETION_VECTOR) {
			target = std::move(source);
			continue;
		}
		auto &target_positions = static_cast<IcebergPositionalDeleteData &>(*target);
		auto &source_positions = static_cast<IcebergPositionalDeleteData &>(*source);
		for (auto &source_entry : source_positions.entries) {
			target_positions.entries.push_back(source_entry);
		}
		target_positions.invalid_rows.insert(source_positions.invalid_rows.begin(),
		                                     source_positions.invalid_rows.end());
	}

	for (auto &entry : scan_result.equality_delete_data) {
		if (entry.delete_file->equality_values.size() == 0) {
			continue;
		}
		lock_guard<mutex> guard(entry.load->lock);
		entry.load->equality_delete = std::move(entry.delete_file);
	}
}

static void CompleteDeleteFileLoads(const vector<shared_ptr<IcebergDeleteFileLoadState>> &loads,
                                    const ErrorData &error) {
	for (auto &load : loads) {
		{
			lock_guard<mutex> guard(load->lock);
			load->error = error;
			load->complete = true;
		}
		load->cv.notify_all();
	}
}

} // namespace

IcebergScanPlanner::IcebergScanPlanner(ClientContext &context_p, shared_ptr<IcebergScanInfo> scan_info,
                                       const string &path, const IcebergOptions &options_p)
    : shared_state(make_shared_ptr<IcebergScanPlanState>(context_p, std::move(scan_info), path, options_p)),
      context(shared_state->context), fs(shared_state->fs), options(shared_state->options) {
}

IcebergScanPlanner::IcebergScanPlanner(shared_ptr<IcebergScanPlanState> shared_state_p)
    : shared_state(std::move(shared_state_p)), context(shared_state->context), fs(shared_state->fs),
      options(shared_state->options) {
}

IcebergScanPlanner::~IcebergScanPlanner() {
}

unique_ptr<IcebergScanPlanner> IcebergScanPlanner::CreateView(IcebergTableFilters filters) const {
	unique_ptr<RowGroupOrderOptions> filtered_scan_order;
	{
		annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
		filtered_scan_order = scan_order.CopyOptions();
	}
	auto result = unique_ptr<IcebergScanPlanner>(new IcebergScanPlanner(shared_state));
	result->table_filters = std::move(filters);
	result->names = names;
	result->types = types;
	result->have_bound = have_bound;
	if (filtered_scan_order) {
		result->SetScanOrder(std::move(filtered_scan_order));
	}
	return result;
}

const string &IcebergScanPlanner::GetPath() const {
	return shared_state->path;
}

const IcebergOptions &IcebergScanPlanner::GetOptions() const {
	return options;
}

const IcebergTableMetadata &IcebergScanPlanner::GetMetadata() const {
	return shared_state->scan_info->metadata;
}

const IcebergTableSchema &IcebergScanPlanner::GetSchema() const {
	return shared_state->scan_info->schema;
}

bool IcebergScanPlanner::HasTransactionData() const {
	return shared_state->scan_info->transaction_data;
}

const IcebergTransactionData &IcebergScanPlanner::GetTransactionData() const {
	D_ASSERT(HasTransactionData());
	return *shared_state->scan_info->transaction_data;
}

const IcebergSnapshotScanInfo &IcebergScanPlanner::GetSnapshot() const {
	return shared_state->scan_info->snapshot_info;
}

optional_ptr<IcebergTableSchemaVersion> IcebergScanPlanner::GetTable() const {
	return shared_state->table;
}

void IcebergScanPlanner::SetTable(IcebergTableSchemaVersion &table) {
	shared_state->table = table;
}

void IcebergScanPlanner::SetOptions(const IcebergOptions &new_options) {
	shared_state->options = new_options;
}

void IcebergScanPlanner::SetScanOrder(unique_ptr<RowGroupOrderOptions> order_options) {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	scan_order.Set(std::move(order_options));
}

void IcebergScanPlanner::DisableServerSidePlanning() {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	if (!shared_state->manifest_list_loaded) {
		shared_state->server_side_planning_enabled = false;
	}
}

void IcebergScanPlanner::Bind(vector<LogicalType> &return_types, vector<Identifier> &return_names) {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	if (have_bound) {
		return_names = StringsToIdentifiers(names);
		return_types = types;
		return;
	}
	if (!shared_state->scan_info) {
		D_ASSERT(!shared_state->path.empty());
		auto resolved_metadata = IcebergUtils::ResolveTableMetadata(context, shared_state->path, options);
		auto temp_data = make_uniq<IcebergScanTemporaryData>();
		temp_data->metadata = std::move(resolved_metadata.metadata);
		auto &metadata = temp_data->metadata;
		auto snapshot_info = metadata.GetSnapshot(*options.snapshot_lookup);
		auto schema = metadata.GetSchemaFromId(snapshot_info.schema_id);
		shared_state->scan_info = make_shared_ptr<IcebergScanInfo>(resolved_metadata.table_location,
		                                                           std::move(temp_data), snapshot_info, *schema);
	}
	for (auto &schema_entry : GetSchema().columns) {
		return_names.push_back(Identifier(schema_entry->name));
		return_types.push_back(schema_entry->type);
	}
	QueryResult::DeduplicateColumns(return_names);
	for (idx_t i = 0; i < return_names.size(); i++) {
		GetSchema().columns[i]->name = return_names[i].GetIdentifierName();
	}
	have_bound = true;
	names = IdentifiersToStrings(return_names);
	types = return_types;
}

const vector<string> &IcebergScanPlanner::Names() const {
	return names;
}

const vector<LogicalType> &IcebergScanPlanner::Types() const {
	return types;
}

bool IcebergScanPlanner::IsBound() const {
	return have_bound;
}

const IcebergTableFilters &IcebergScanPlanner::Filters() const {
	return table_filters;
}

IcebergScanPlanProvider &IcebergScanPlanner::GetScanPlanProvider() const {
	D_ASSERT(scan_plan_provider);
	return *scan_plan_provider;
}

IcebergScanPlanContext IcebergScanPlanner::GetScanPlanContext() const {
	optional_ptr<const IcebergTransactionData> transaction_data;
	if (HasTransactionData()) {
		transaction_data = &GetTransactionData();
	}
	return {context, fs, GetPath(), options, GetSnapshot(), GetMetadata(), GetSchema(), transaction_data};
}

IcebergDeletePlanningContext IcebergScanPlanner::GetDeletePlanningContext() const {
	return {context,
	        fs,
	        GetPath(),
	        options,
	        GetMetadata(),
	        GetSchema(),
	        table_filters,
	        data_manifests,
	        delete_manifests,
	        delete_manifest_matches,
	        GetScanPlanProvider()};
}

void IcebergScanPlanner::InitializeScanPlanProvider() const {
	if (!scan_plan_provider) {
		scan_plan_provider =
		    IcebergScanPlanProvider::Create(*shared_state, GetScanPlanContext(), GetTable(), table_filters, scan_order,
		                                    shared_state->server_side_planning_enabled);
	}
}

void IcebergScanPlanner::LoadManifestList(annotated_lock_guard<annotated_mutex> &) const {
	InitializeScanPlanProvider();
	GetScanPlanProvider().LoadManifestList();
}

void IcebergScanPlanner::InitializeView(annotated_lock_guard<annotated_mutex> &guard) const {
	if (scan_plan_provider) {
		return;
	}
	LoadManifestList(guard);
	IcebergFilePruner pruner(context, GetMetadata(), GetSchema(), table_filters);
	auto &committed_data = GetScanPlanProvider().DataManifests();
	for (auto &manifest : committed_data) {
		data_manifests.emplace_back(data_manifests.size(), manifest);
		data_manifest_matches.push_back(pruner.ManifestMatchesFilter(manifest.file));
	}
	for (auto &manifest : shared_state->transaction_data_manifests) {
		data_manifests.emplace_back(data_manifests.size(), manifest);
		data_manifest_matches.push_back(pruner.ManifestMatchesFilter(manifest.get().file));
	}
	auto &committed_deletes = GetScanPlanProvider().DeleteManifests();
	bool has_matching_deletes = false;
	for (auto &manifest : committed_deletes) {
		delete_manifests.emplace_back(delete_manifests.size(), manifest);
		auto matches = pruner.ManifestMatchesFilter(manifest.file);
		delete_manifest_matches.push_back(matches);
		has_matching_deletes |= matches;
	}
	for (auto &manifest : shared_state->transaction_delete_manifests) {
		delete_manifests.emplace_back(delete_manifests.size(), manifest);
		auto matches = pruner.ManifestMatchesFilter(manifest.get().file);
		delete_manifest_matches.push_back(matches);
		has_matching_deletes |= matches;
	}
	has_matching_delete_manifests.store(has_matching_deletes);
}

void IcebergScanPlanner::StartDataManifestScan(annotated_lock_guard<annotated_mutex> &) const {
	D_ASSERT(scan_plan_provider);
	GetScanPlanProvider().StartDataManifestScan(data_manifest_matches, table_filters.FilterCount());
}

bool IcebergScanPlanner::TryGetNextBatch(annotated_lock_guard<annotated_mutex> &) const {
	return GetScanPlanProvider().TryGetNextBatch(data_view_cursor);
}

void IcebergScanPlanner::FinishScanTasks(annotated_lock_guard<annotated_mutex> &) const {
	GetScanPlanProvider().FinishScanTasks();
}

optional_ptr<const BoundIcebergManifestEntry>
IcebergScanPlanner::GetDataFile(idx_t file_id, annotated_lock_guard<annotated_mutex> &guard) const {
	InitializeView(guard);
	StartDataManifestScan(guard);
	if (file_id < data_manifest_entries.size()) {
		return data_manifest_entries[file_id];
	}
	while (file_id >= data_manifest_entries.size()) {
		if (!TryGetNextBatch(guard)) {
			FinishScanTasks(guard);
			return nullptr;
		}
		auto &batch = data_view_cursor.current_batch;
		auto &bound_manifest = data_manifests[batch.manifest_list_entry_idx];
		auto &manifest_entries = bound_manifest.entry.GetManifestEntries();
		auto &manifest_file = bound_manifest.entry.file;
		if (!data_manifest_matches[batch.manifest_list_entry_idx]) {
			data_view_cursor.current_batch_offset = batch.end_index;
		}
		for (; data_view_cursor.current_batch_offset < batch.end_index && file_id >= data_manifest_entries.size();
		     data_view_cursor.current_batch_offset++) {
			auto &manifest_entry = manifest_entries[data_view_cursor.current_batch_offset];
			auto &data_file = manifest_entry.data_file;
			auto entry_path = data_file.file_path;
			if (options.allow_moved_paths) {
				entry_path = IcebergUtils::GetFullPath(GetPath(), entry_path, fs);
			}
			IcebergPartition partition {manifest_file.partition_spec_id, data_file.partition_info};
			shared_state->data_file_partitions[entry_path] = partition;
			shared_state->data_file_partitions[data_file.file_path] = std::move(partition);
			auto bound_entry = bound_manifest.BindEntry(manifest_entry);
			if (manifest_entry.status == IcebergManifestEntryStatusType::DELETED) {
				continue;
			}
			if (table_filters.HasFilters() && !IcebergFilePruner(context, GetMetadata(), GetSchema(), table_filters)
			                                       .FileMatchesFilter(manifest_file, manifest_entry)) {
				continue;
			}
			if (StringUtil::CIEquals(data_file.file_format, "puffin")) {
				continue;
			}
			data_manifest_entries.push_back(bound_entry);
		}
		if (data_view_cursor.current_batch_offset >= batch.end_index) {
			data_view_cursor.has_current_batch = false;
		}
	}
	return data_manifest_entries[file_id];
}

void IcebergScanPlanner::EnsureScanOrderApplied(annotated_lock_guard<annotated_mutex> &guard) const {
	if (!scan_order.IsPending()) {
		return;
	}
	idx_t materialized = 0;
	while (GetDataFile(materialized, guard)) {
		materialized++;
	}
	scan_order.Apply(context, GetSchema(), has_matching_delete_manifests.load(), data_manifest_entries);
}

optional_ptr<const BoundIcebergManifestEntry> IcebergScanPlanner::GetDataFile(idx_t file_id) const {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	GetDataFile(file_id, guard);
	EnsureScanOrderApplied(guard);
	return file_id < data_manifest_entries.size()
	           ? optional_ptr<const BoundIcebergManifestEntry>(data_manifest_entries[file_id])
	           : nullptr;
}

const IcebergManifestFile &IcebergScanPlanner::GetManifestFileForEntry(const BoundIcebergManifestEntry &entry,
                                                                       IcebergManifestContentType type) const {
	return type == IcebergManifestContentType::DATA ? data_manifests[entry.manifest_file_idx].entry.file
	                                                : delete_manifests[entry.manifest_file_idx].entry.file;
}

optional<IcebergScanTask> IcebergScanPlanner::GetDataFileTask(idx_t file_id) const {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	GetDataFile(file_id, guard);
	EnsureScanOrderApplied(guard);
	if (file_id >= data_manifest_entries.size()) {
		return nullopt;
	}
	auto data_file = data_manifest_entries[file_id];
	auto manifest_file = GetManifestFileForEntry(data_file, IcebergManifestContentType::DATA);
	auto path = data_file.entry.data_file.file_path;
	if (options.allow_moved_paths) {
		path = IcebergUtils::GetFullPath(GetPath(), path, fs);
	}
	return IcebergScanTask {data_file, std::move(manifest_file), std::move(path), {}};
}

optional<IcebergScanTask> IcebergScanPlanner::GetScanTask(idx_t file_id) const {
	auto task = GetDataFileTask(file_id);
	if (task) {
		task->delete_files = ResolveApplicableDeleteFiles(task->data_file);
	}
	return task;
}

idx_t IcebergScanPlanner::GetTotalFileCount() const {
	idx_t file_id = 0;
	while (GetDataFileTask(file_id)) {
		file_id++;
	}
	return file_id;
}

unique_ptr<NodeStatistics> IcebergScanPlanner::GetCardinality() const {
	if (GetMetadata().iceberg_version == 1) {
		return nullptr;
	}
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	InitializeView(guard);
	idx_t cardinality = 0;
	for (idx_t i = 0; i < data_manifests.size(); i++) {
		auto &manifest = data_manifests[i].entry.file;
		if (!data_manifest_matches[i]) {
			continue;
		}
		if (!manifest.counts || !manifest.counts->added_rows_count || !manifest.counts->existing_rows_count) {
			return nullptr;
		}
		cardinality += *manifest.counts->added_rows_count + *manifest.counts->existing_rows_count;
	}
	for (idx_t i = 0; i < delete_manifests.size(); i++) {
		auto &manifest = delete_manifests[i].entry.file;
		if (!delete_manifest_matches[i]) {
			continue;
		}
		if (!manifest.counts || !manifest.counts->added_rows_count) {
			return nullptr;
		}
		cardinality -= *manifest.counts->added_rows_count;
	}
	return make_uniq<NodeStatistics>(cardinality, cardinality);
}

void IcebergScanPlanner::GetStatistics(vector<PartitionStatistics> &result) const {
	if (GetMetadata().iceberg_version == 1) {
		return;
	}
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	InitializeView(guard);
	for (idx_t i = 0; i < delete_manifests.size(); i++) {
		if (delete_manifest_matches[i]) {
			return;
		}
	}
	idx_t count = 0;
	for (idx_t i = 0; i < data_manifests.size(); i++) {
		auto &manifest = data_manifests[i].entry.file;
		if (!data_manifest_matches[i]) {
			continue;
		}
		if (!manifest.counts || !manifest.counts->added_rows_count || !manifest.counts->existing_rows_count) {
			return;
		}
		count += *manifest.counts->existing_rows_count + *manifest.counts->added_rows_count;
	}
	PartitionStatistics stats;
	stats.count = count;
	stats.count_type = CountType::COUNT_EXACT;
	result.push_back(stats);
}

IcebergPartition IcebergScanPlanner::GetPartitionForDataFile(const string &file_path) const {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	auto entry = shared_state->data_file_partitions.find(file_path);
	if (entry != shared_state->data_file_partitions.end()) {
		return entry->second;
	}
	throw InvalidConfigurationException("Could not find data file '%s' in manifest entries", file_path);
}

vector<IcebergDeleteFileReference>
IcebergScanPlanner::ResolveApplicableDeleteFiles(const BoundIcebergManifestEntry &data_manifest_entry) const {
	vector<IcebergDeleteFileReference> result;
	if (!has_matching_delete_manifests.load()) {
		return result;
	}
	vector<idx_t> manifest_indexes;
	optional_ptr<IcebergScanPlanProvider> provider;
	{
		annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
		InitializeView(guard);
		manifest_indexes =
		    IcebergDeletePlanner::GetDeleteManifestsForDataFile(GetDeletePlanningContext(), data_manifest_entry);
		provider = scan_plan_provider.get();
	}
	if (manifest_indexes.empty()) {
		return result;
	}
	provider->ReadDeleteManifests(manifest_indexes, table_filters.FilterCount());
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	annotated_lock_guard<annotated_mutex> delete_guard(shared_state->delete_lock);
	auto delete_context = GetDeletePlanningContext();
	auto partition_values = IcebergFilePruner::PartitionValueMap(data_manifest_entry.entry.data_file);
	for (auto delete_file : provider->GetDeleteFiles(manifest_indexes)) {
		if (delete_file.manifest_idx >= delete_manifests.size()) {
			throw InternalException("Delete manifest index %llu is out of bounds for %llu manifests",
			                        delete_file.manifest_idx, delete_manifests.size());
		}
		auto &entries = delete_manifests[delete_file.manifest_idx].entry.GetManifestEntries();
		if (delete_file.entry_idx >= entries.size()) {
			throw InternalException("Delete manifest entry index %llu is out of bounds for manifest %llu",
			                        delete_file.entry_idx, delete_file.manifest_idx);
		}
		auto &delete_entry = entries[delete_file.entry_idx];
		if (IcebergDeletePlanner::DeleteEntryMatchesFilters(delete_context, delete_file.manifest_idx, delete_entry) &&
		    IcebergDeletePlanner::DeleteEntryAppliesToDataFile(delete_context, delete_file.manifest_idx, delete_entry,
		                                                       data_manifest_entry, partition_values)) {
			result.push_back(delete_file);
		}
	}
	return result;
}

IcebergDeletePlan IcebergScanPlanner::ProcessDeletes(const IcebergScanTask &task) const {
	IcebergDeletePlan result;
	if (task.delete_files.empty()) {
		return result;
	}
	optional_ptr<IcebergScanPlanProvider> provider;
	vector<IcebergDeleteScanEntry> scan_entries;
	vector<shared_ptr<IcebergDeleteFileLoadState>> required_loads;
	vector<shared_ptr<IcebergDeleteFileLoadState>> new_loads;
	unique_ptr<IcebergDeletePlanningContext> delete_context;
	{
		annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
		annotated_lock_guard<annotated_mutex> delete_guard(shared_state->delete_lock);
		provider = scan_plan_provider.get();
		if (!provider) {
			throw InternalException("scan_plan_provider is not initialized in ProcessDeletes");
		}
		delete_context = make_uniq<IcebergDeletePlanningContext>(GetDeletePlanningContext());
		unordered_set<IcebergDeleteFileLoadState *> seen_loads;
		for (auto delete_file : task.delete_files) {
			auto &delete_manifest = delete_manifests[delete_file.manifest_idx].entry;
			auto &load = provider->GetDeleteFileLoad(delete_file);
			if (!load) {
				load = make_shared_ptr<IcebergDeleteFileLoadState>();
				new_loads.push_back(load);
				scan_entries.emplace_back(delete_file.manifest_idx, delete_file.entry_idx, delete_manifest, load);
			}
			if (seen_loads.insert(load.get()).second) {
				required_loads.push_back(load);
			}
		}
	}
	if (!scan_entries.empty()) {
		ErrorData scan_error;
		try {
			auto scan_result = IcebergDeleteFileScanner::ScanFiles(*delete_context, scan_entries);
			annotated_lock_guard<annotated_mutex> delete_guard(shared_state->delete_lock);
			MergeDeleteScanResult(*provider, std::move(scan_result));
		} catch (std::exception &ex) {
			scan_error = ErrorData(ex);
		} catch (...) { // LCOV_EXCL_START
			scan_error = ErrorData("Unknown exception while reading Iceberg delete files");
		} // LCOV_EXCL_STOP
		CompleteDeleteFileLoads(new_loads, scan_error);
	}
	for (auto &load : required_loads) {
		unique_lock<mutex> guard(load->lock);
		load->cv.wait(guard, [&load] { return load->complete; });
		if (load->error.HasError()) {
			load->error.Throw();
		}
		if (load->equality_delete) {
			result.equality_deletes.emplace_back(*load->equality_delete);
		}
	}
	{
		annotated_lock_guard<annotated_mutex> delete_guard(shared_state->delete_lock);
		auto &positional_data = provider->PositionalDeleteData();
		auto entry = positional_data.find(task.data_file.entry.data_file.file_path);
		if (entry != positional_data.end()) {
			result.positional_deletes = entry->second->ToFilter();
		}
	}
	return result;
}

shared_ptr<IcebergDeleteData> IcebergScanPlanner::GetExistingPositionalDeleteData(const string &file_path) const {
	annotated_lock_guard<annotated_mutex> guard(shared_state->lock);
	annotated_lock_guard<annotated_mutex> delete_guard(shared_state->delete_lock);
	return IcebergDeletePlanner::GetExistingPositionalDeleteData(GetDeletePlanningContext(), file_path);
}

} // namespace duckdb
