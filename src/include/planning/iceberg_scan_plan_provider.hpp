#pragma once

#include "planning/iceberg_multi_file_list.hpp"
#include "catalog/rest/api/iceberg_scan_planning.hpp"

namespace duckdb {

class IcebergScanPlanProvider {
public:
	virtual ~IcebergScanPlanProvider() = default;

	virtual void LoadManifestList(const IcebergMultiFileList &file_list) = 0;
	virtual void StartDataManifestScan(const IcebergMultiFileList &file_list) = 0;
	virtual void EnumerateDeleteManifestEntries(const IcebergMultiFileList &file_list) = 0;
	virtual bool TryGetNextBatch(IcebergDataViewCursor &cursor) = 0;
	virtual void FinishScanTasks() = 0;
	virtual bool DeleteFileAppliesToDataFile(const string &data_file_path, const string &delete_file_path) const = 0;
	virtual vector<IcebergManifestListEntry> &DataManifests() = 0;
	virtual vector<IcebergManifestListEntry> &DeleteManifests() = 0;
	virtual case_insensitive_map_t<shared_ptr<IcebergDeleteData>> &PositionalDeleteData() = 0;
	virtual map<sequence_number_t, unique_ptr<IcebergEqualityDeleteData>> &EqualityDeleteData() = 0;
	virtual unordered_set<string> &ProcessedDeleteFiles() = 0;
};

class ClientSideScanPlanProvider final : public IcebergScanPlanProvider {
public:
	explicit ClientSideScanPlanProvider(IcebergMultiFileListSharedState &shared_state);

	void LoadManifestList(const IcebergMultiFileList &file_list) override;
	void StartDataManifestScan(const IcebergMultiFileList &file_list) override;
	void EnumerateDeleteManifestEntries(const IcebergMultiFileList &file_list) override;
	bool TryGetNextBatch(IcebergDataViewCursor &cursor) override;
	void FinishScanTasks() override;
	bool DeleteFileAppliesToDataFile(const string &data_file_path, const string &delete_file_path) const override;
	vector<IcebergManifestListEntry> &DataManifests() override;
	vector<IcebergManifestListEntry> &DeleteManifests() override;
	case_insensitive_map_t<shared_ptr<IcebergDeleteData>> &PositionalDeleteData() override;
	map<sequence_number_t, unique_ptr<IcebergEqualityDeleteData>> &EqualityDeleteData() override;
	unordered_set<string> &ProcessedDeleteFiles() override;

private:
	void StartDeleteManifestScan(const IcebergMultiFileList &file_list);
	bool FinishedScanningDeletes() const;

private:
	IcebergMultiFileListSharedState &shared_state;
	unique_ptr<AvroScan> delete_manifest_scan;
	unique_ptr<manifest_file::ManifestReader> delete_manifest_reader;
};

class ServerSideScanPlanProvider final : public IcebergScanPlanProvider {
public:
	explicit ServerSideScanPlanProvider(IcebergServerSideScanPlan plan);

	void LoadManifestList(const IcebergMultiFileList &file_list) override;
	void StartDataManifestScan(const IcebergMultiFileList &file_list) override;
	void EnumerateDeleteManifestEntries(const IcebergMultiFileList &file_list) override;
	bool TryGetNextBatch(IcebergDataViewCursor &cursor) override;
	void FinishScanTasks() override;
	bool DeleteFileAppliesToDataFile(const string &data_file_path, const string &delete_file_path) const override;
	vector<IcebergManifestListEntry> &DataManifests() override;
	vector<IcebergManifestListEntry> &DeleteManifests() override;
	case_insensitive_map_t<shared_ptr<IcebergDeleteData>> &PositionalDeleteData() override;
	map<sequence_number_t, unique_ptr<IcebergEqualityDeleteData>> &EqualityDeleteData() override;
	unordered_set<string> &ProcessedDeleteFiles() override;

private:
	//! Declared before bound entries and parsed delete data so their references are destroyed first.
	IcebergServerSideScanPlan plan;
	ManifestEntryReadState read_state;
	bool data_manifest_scan_started = false;
	case_insensitive_map_t<shared_ptr<IcebergDeleteData>> positional_delete_data;
	map<sequence_number_t, unique_ptr<IcebergEqualityDeleteData>> equality_delete_data;
	unordered_set<string> processed_delete_files;
};

} // namespace duckdb
