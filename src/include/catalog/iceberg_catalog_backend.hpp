#pragma once

#include "duckdb/common/case_insensitive_map.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/unique_ptr.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/set.hpp"

namespace duckdb {
class IcebergScanPlanProvider;
struct IcebergScanPlanContext;
struct IcebergTableFilters;
class IcebergScanOrder;
class ClientContext;
class ErrorData;
class IcebergSchemaEntry;
class IcebergTransaction;
struct IcebergTable;
struct IcebergCreateTableInfo;
struct IcebergTransactionAlterUpdate;
struct IcebergTableCredentials;
struct SecretEntry;

//! Immutable backend-specific state retained by a table and its transaction-local copies.
struct IcebergTableBackendData {
	virtual ~IcebergTableBackendData() = default;
};

//! Catalog operations independent of the transport used to store Iceberg metadata.
//! The catalog owns this backend; schemas, tables and transactions share it for its lifetime.
class IcebergCatalogBackend {
public:
	virtual ~IcebergCatalogBackend() = default;
	virtual void LoadCredentials(ClientContext &context, const IcebergTable &table) = 0;
	virtual string GetTableKey(const vector<string> &schema, const string &table) const = 0;
	virtual unique_ptr<IcebergScanPlanProvider> PlanScan(IcebergScanPlanContext &context, IcebergTable &table,
	                                                     const IcebergTableFilters &filters,
	                                                     const IcebergScanOrder &order) = 0;
	virtual string GetDescription() const = 0;
	virtual string GetDBPath() const = 0;
	virtual bool SchemaExists(ClientContext &context, const string &name) = 0;
	virtual vector<vector<string>> ListSchemas(ClientContext &context) = 0;
	//! A refused listing returns nullopt, so callers do not interpret it as an empty schema.
	virtual optional<vector<string>> ListTables(ClientContext &context, const IcebergSchemaEntry &schema) = 0;
	virtual bool TableExists(ClientContext &context, const IcebergSchemaEntry &schema, const string &name) = 0;
	virtual void LoadSchemaProperties(ClientContext &context, IcebergSchemaEntry &schema) = 0;
	//! Return false only for a missing table. Refresh bypasses cached metadata and replaces existing schemas.
	virtual bool LoadTable(ClientContext &context, IcebergTable &table, bool refresh = false) = 0;
	//! Initialize and register the new table in the transaction, including backend-specific creation requirements.
	virtual IcebergTable &CreateTable(ClientContext &context, IcebergTransaction &transaction, IcebergTable &&table,
	                                  const IcebergCreateTableInfo &request) = 0;
	virtual void InvalidateTable(const IcebergTable &table) = 0;
	virtual void CreateSchema(ClientContext &context, const vector<string> &name) = 0;
	virtual void DropSchema(ClientContext &context, const vector<string> &name) = 0;
	virtual void UpdateSchemaProperties(ClientContext &context, const vector<string> &name,
	                                    const case_insensitive_map_t<string> &updates, const set<string> &removals) = 0;
	virtual void DropTable(ClientContext &context, const IcebergTable &table) = 0;
	virtual void RenameTable(ClientContext &context, const IcebergTable &table, const string &new_name) = 0;
	virtual bool SupportsMultiTableCommit() const = 0;
	virtual bool CanCommitMultipleTables(const IcebergTransactionAlterUpdate &update) const = 0;
	virtual void CommitTableUpdates(ClientContext &context, IcebergTransactionAlterUpdate &update) = 0;
	//! Unknown outcomes must retain written files because the commit may have succeeded.
	virtual bool CommitStateUnknown(const ErrorData &error) const = 0;
	virtual IcebergTableCredentials GetTableCredentials(ClientContext &context, const IcebergTable &table) = 0;
	virtual IcebergTableCredentials RefreshTableCredentials(ClientContext &context, const IcebergTable &table) = 0;
	virtual unique_ptr<SecretEntry> GetHTTPSecret(ClientContext &context) = 0;
};

} // namespace duckdb
