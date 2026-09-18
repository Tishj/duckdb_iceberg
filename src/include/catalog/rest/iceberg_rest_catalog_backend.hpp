#pragma once

#include "catalog/iceberg_catalog_backend.hpp"
#include "catalog/rest/api/url_utils.hpp"
#include "catalog/rest/storage/iceberg_authorization.hpp"
#include "common/iceberg_utils.hpp"
#include "rest_catalog/objects/load_table_result.hpp"

namespace duckdb {
class IcebergCatalog;

struct IcebergRESTTableData : public IcebergTableBackendData {
	//! Identity of the response used to load this table; never dereferenced.
	optional_ptr<const rest_api_objects::LoadTableResult> initialization_source;
	vector<rest_api_objects::StorageCredential> storage_credentials;
};

class MetadataCacheValue {
public:
	MetadataCacheValue(timestamp_ms_t expire_timestamp_ms,
	                   unique_ptr<const rest_api_objects::LoadTableResult> load_table_result)
	    : expire_timestamp_ms(expire_timestamp_ms), load_table_result(std::move(load_table_result)) {
	}

public:
	//! The timestamp until when this entry is valid
	timestamp_ms_t expire_timestamp_ms;
	//! The payload of the cache entry
	unique_ptr<const rest_api_objects::LoadTableResult> load_table_result;
};

class LoadTableResultCache {
public:
	LoadTableResultCache(IcebergAttachOptions &attach_options) : attach_options(attach_options) {
	}

public:
	bool Get(ClientContext &context, const string &table_key,
	         const std::function<void(const rest_api_objects::LoadTableResult &)> &callback,
	         bool validate_cache = true) {
		annotated_lock_guard<annotated_mutex> guard(lock);
		auto it = tables.find(table_key);
		if (it == tables.end()) {
			return false;
		}

		auto transaction_start_ms = IcebergUtils::GetTransactionStartTimeMS(context);

		auto &entry = it->second;
		if (validate_cache && transaction_start_ms > entry.expire_timestamp_ms) {
			// cached value has expired
			return false;
		}
		callback(*entry.load_table_result);
		return true;
	}
	void SetOrOverwrite(const string &table_key,
	                    unique_ptr<const rest_api_objects::LoadTableResult> load_table_result) {
		annotated_lock_guard<annotated_mutex> guard(lock);
		// If max_table_staleness_minutes is not set, use a time in the past so cache is always expired
		system_clock::time_point expires_at;
		if (attach_options.max_table_staleness_micros.IsValid()) {
			expires_at =
			    system_clock::now() + std::chrono::microseconds(attach_options.max_table_staleness_micros.GetIndex());
		} else {
			expires_at = system_clock::time_point::min();
		}
		auto epoch_micros = timestamp_t(duration_cast<microseconds>(expires_at.time_since_epoch()).count());
		auto expire_timestamp_ms = timestamp_ms_t(Timestamp::GetEpochMs(epoch_micros));

		// erase load table result if it exists.
		tables.erase(table_key);
		tables.emplace(table_key, MetadataCacheValue(expire_timestamp_ms, std::move(load_table_result)));
	}

	//! Evict only if the table was initialized from the result that is still cached for its key.
	void EvictIfCurrent(const IcebergTable &table);

private:
	IcebergAttachOptions &attach_options;
	annotated_mutex lock;
	case_insensitive_map_t<MetadataCacheValue> tables DUCKDB_GUARDED_BY(lock);
};

//! REST configuration, transport, response caching and commit protocol.
class IcebergRESTCatalogBackend : public IcebergCatalogBackend {
public:
	IcebergRESTCatalogBackend(unique_ptr<IcebergAuthorization> auth_handler, const IcebergAttachOptions &options);
	~IcebergRESTCatalogBackend() override;
	void LoadCredentials(ClientContext &context, const IcebergTable &table) override;
	string GetTableKey(const vector<string> &schema, const string &table) const override;
	unique_ptr<IcebergScanPlanProvider> PlanScan(IcebergScanPlanContext &context, IcebergTable &table,
	                                             const IcebergTableFilters &filters,
	                                             const IcebergScanOrder &order) override;
	string GetDescription() const override {
		return "Iceberg REST Catalog";
	}
	string GetDBPath() const override;
	bool SchemaExists(ClientContext &context, const string &name) override;
	vector<vector<string>> ListSchemas(ClientContext &context) override;
	optional<vector<string>> ListTables(ClientContext &context, const IcebergSchemaEntry &schema) override;
	bool TableExists(ClientContext &context, const IcebergSchemaEntry &schema, const string &name) override;
	void LoadSchemaProperties(ClientContext &context, IcebergSchemaEntry &schema) override;
	//! Return false only for a missing table. Refresh bypasses cached metadata and replaces existing schemas.
	bool LoadTable(ClientContext &context, IcebergTable &table, bool refresh = false) override;
	IcebergTable &CreateTable(ClientContext &context, IcebergTransaction &transaction, IcebergTable &&table,
	                          const IcebergCreateTableInfo &request) override;
	void InvalidateTable(const IcebergTable &table) override;
	void CreateSchema(ClientContext &context, const vector<string> &name) override;
	void DropSchema(ClientContext &context, const vector<string> &name) override;
	void UpdateSchemaProperties(ClientContext &context, const vector<string> &name,
	                            const case_insensitive_map_t<string> &updates, const set<string> &removals) override;
	void DropTable(ClientContext &context, const IcebergTable &table) override;
	void RenameTable(ClientContext &context, const IcebergTable &table, const string &new_name) override;
	bool SupportsMultiTableCommit() const override;
	bool CanCommitMultipleTables(const IcebergTransactionAlterUpdate &update) const override;
	void CommitTableUpdates(ClientContext &context, IcebergTransactionAlterUpdate &update) override;
	//! Unknown outcomes must retain written files because the commit may have succeeded.
	bool CommitStateUnknown(const ErrorData &error) const override;
	IcebergTableCredentials GetTableCredentials(ClientContext &context, const IcebergTable &table) override;
	IcebergTableCredentials RefreshTableCredentials(ClientContext &context, const IcebergTable &table) override;
	unique_ptr<SecretEntry> GetHTTPSecret(ClientContext &context) override;
	IcebergTableCredentials MakeTableCredentials(ClientContext &context, const IcebergTable &table,
	                                             const vector<rest_api_objects::StorageCredential> &credentials);
	static IcebergRESTCatalogBackend &Get(IcebergCatalog &catalog);
	static unique_ptr<SecretEntry> GetStorageSecret(ClientContext &context, const string &secret_name);
	static unique_ptr<SecretEntry> GetIcebergSecret(ClientContext &context, const string &secret_name);
	static unique_ptr<SecretEntry> GetHTTPSecret(ClientContext &context, const string &secret_name);
	void GetConfig(ClientContext &context, IcebergEndpointType &endpoint_type);
	IRCEndpointBuilder GetBaseUrl() const;

public:
	unique_ptr<IcebergAuthorization> auth_handler;
	string base_uri;
	const string version = "v1";
	vector<string> prefix;
	string namespace_separator = "\x1f";
	IcebergAttachOptions attach_options;
	unordered_set<string> supported_urls;

private:
	void InitializeTable(IcebergTable &table, const rest_api_objects::LoadTableResult &result);
	void InstallTableCredentials(ClientContext &context, const IcebergTable &table,
	                             IcebergTableCredentials credentials);
	void ParsePrefix();
	void ParseNamespaceSeparator();
	void AddDefaultSupportedEndpoints();
	void AddS3TablesEndpoints();
	void AddGlueEndpoints();
	void DoMultiTableCommitUpdates(IcebergTransactionAlterUpdate &update, ClientContext &context);
	void DoSingleTableCommitUpdates(IcebergTransactionAlterUpdate &update, ClientContext &context);
	void CleanupMetadataFiles(ClientContext &context, const vector<string> &paths);
	void RefreshRetryTables(IcebergTransactionAlterUpdate &update, const case_insensitive_set_t &keys,
	                        ClientContext &context);
	string warehouse;
	case_insensitive_map_t<string> defaults;
	case_insensitive_map_t<string> overrides;
	LoadTableResultCache table_request_cache;
};

} // namespace duckdb
