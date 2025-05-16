
#pragma once

#include "duckdb/transaction/transaction.hpp"
#include "rest_catalog/objects/list_namespaces_response.hpp"
#include "rest_catalog/objects/list_tables_response.hpp"
#include "rest_catalog/objects/load_table_result.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/table_metadata.hpp"
#include "rest_catalog/objects/storage_credential.hpp"
#include "duckdb/planner/tableref/bound_at_clause.hpp"
#include "iceberg_metadata.hpp"

namespace duckdb {
class IRCatalog;
class IRCSchemaEntry;
class ICTableEntry;
class IRCSchemaEntry;

enum class IRCTransactionState { TRANSACTION_NOT_YET_STARTED, TRANSACTION_STARTED, TRANSACTION_FINISHED };

enum class IRCNamespaceState { MISSING, EXISTS, UNKNOWN };

//! Table Schema
struct IcebergSchemaInformation {
public:
	IcebergSchemaInformation(rest_api_objects::Schema &schema);

public:
	int32_t schema_id;
	vector<unique_ptr<IcebergColumnDefinition>> columns;
};

//! Table Metadata
struct IcebergTableMetadata {
public:
	IcebergTableMetadata(rest_api_objects::TableMetadata &&metadata);

public:
	const rest_api_objects::Snapshot &GetSnapshot(const BoundAtClause &at) const;

private:
	int64_t SnapshotFromTimestamp(timestamp_t timestamp) const;

public:
	rest_api_objects::TableMetadata metadata;
	//! schema_id -> table entry
	unordered_map<int32_t, IcebergSchemaInformation> table_schemas;
	//! snapshot_id -> snapshot info
	unordered_map<int64_t, rest_api_objects::Snapshot> table_snapshots;
	//! timestamp_ms -> snapshot_id
	map<int64_t, int64_t> timestamp_to_snapshot;
};

struct IcebergTableInformation {
public:
	IcebergTableInformation(IcebergTableMetadata &&metadata, IRCSchemaEntry &schema, const string &name);

public:
	optional_ptr<ICTableEntry> GetTableSchema(optional_ptr<BoundAtClause> at);

public:
	IcebergTableMetadata table_metadata;
	IRCSchemaEntry &schema;
	string name;
	case_insensitive_map_t<string> config;
	vector<rest_api_objects::StorageCredential> storage_credentials;

	//! schema_id -> table entry
	unordered_map<int32_t, unique_ptr<ICTableEntry>> schema_versions;
};

struct IRCNamespaceInformation {
public:
	IRCNamespaceInformation(Catalog &catalog, const string &name);

public:
	unique_ptr<IRCSchemaEntry> catalog_entry;

	bool has_tables = false;
	rest_api_objects::ListTablesResponse tables;

	case_insensitive_map_t<IcebergTableInformation> table_info;
	IRCNamespaceState state = IRCNamespaceState::UNKNOWN;
};

class IRCTransaction : public Transaction {
public:
	IRCTransaction(IRCatalog &ic_catalog, TransactionManager &manager, ClientContext &context);
	~IRCTransaction() override;

	void Start();
	void Commit();
	void Rollback();

	static IRCTransaction &Get(ClientContext &context, Catalog &catalog);
	AccessMode GetAccessMode() const {
		return access_mode;
	}
	optional_ptr<IRCSchemaEntry> GetSchema(ClientContext &context, const string &schema);
	optional_ptr<ICTableEntry> GetTable(ClientContext &context, IRCSchemaEntry &schema, const EntryLookupInfo &lookup);

private:
	IRCatalog &catalog;
	IRCTransactionState transaction_state;
	AccessMode access_mode;

	//! Used when a listing of all available namespaces is requested
	bool has_namespace_listing = false;
	rest_api_objects::ListNamespacesResponse namespace_listing;

	case_insensitive_map_t<IRCNamespaceInformation> namespaces;
};

} // namespace duckdb
