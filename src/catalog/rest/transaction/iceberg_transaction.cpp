#include "catalog/rest/transaction/iceberg_transaction.hpp"
#include "catalog/iceberg_catalog_backend.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/api/iceberg_add_snapshot.hpp"
#include "duckdb/main/connection.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/logging/logger.hpp"
#include "common/iceberg_utils.hpp"
#include "iceberg_logging.hpp"

namespace duckdb {

IcebergTransactionTableState::IcebergTransactionTableState() : status(IcebergTableStatus::MISSING) {
}

IcebergTransactionTableState::IcebergTransactionTableState(shared_ptr<IcebergTable> catalog_table)
    : catalog_table(std::move(catalog_table)), status(IcebergTableStatus::ALIVE) {
}

IcebergTransactionTableState::IcebergTransactionTableState(IcebergTable &&transaction_table_p)
    : transaction_table(make_uniq<IcebergTable>(std::move(transaction_table_p))), status(IcebergTableStatus::ALIVE) {
	if (!transaction_table->table_metadata.GetSchemas().IsEmpty()) {
		transaction_table->InitSchemaVersions();
	}
}

const IcebergTable &IcebergTransactionTableState::GetInfo() const {
	return const_cast<IcebergTransactionTableState &>(*this).GetInfo();
}

IcebergTable &IcebergTransactionTableState::GetOrCreateTransactionInfo(IcebergTransaction &transaction) {
	if (transaction_table) {
		return *transaction_table;
	}
	if (!catalog_table) {
		throw InternalException("Cannot materialize transaction table state without table information");
	}
	transaction_table = make_uniq<IcebergTable>(catalog_table->Copy(transaction));
	transaction_table->InitSchemaVersions();
	return *transaction_table;
}

IcebergTransaction::IcebergTransaction(IcebergCatalog &ic_catalog, TransactionManager &manager, ClientContext &context)
    : Transaction(manager, context), db(*context.db), catalog(ic_catalog), access_mode(ic_catalog.access_mode) {
}

IcebergTransaction::~IcebergTransaction() = default;

void IcebergTransaction::Start() {
}

IcebergCatalog &IcebergTransaction::GetCatalog() {
	return catalog;
}

void IcebergTransaction::VerifyAlterUpdateAtomicity(const IcebergTransactionAlterUpdate &alter_update) const {
	if (alter_update.updated_tables.size() <= 1) {
		return;
	}
	if (catalog.GetBackend().CanCommitMultipleTables(alter_update)) {
		return;
	}
	throw TransactionException("%s cannot commit this transaction atomically because it would "
	                           "require multiple table commit requests without atomic multi-table commit support",
	                           catalog.GetBackend().GetDescription());
}

bool IcebergTransaction::MultiTableCommitAvailable() const {
	return catalog.GetBackend().SupportsMultiTableCommit();
}

bool IcebergTransaction::HasTableUpdate() const {
	return !std::holds_alternative<std::monostate>(transaction_update);
}

IcebergTransactionAlterUpdate *IcebergTransaction::GetAlterUpdate() {
	return std::get_if<IcebergTransactionAlterUpdate>(&transaction_update);
}

const IcebergTransactionAlterUpdate *IcebergTransaction::GetAlterUpdate() const {
	return std::get_if<IcebergTransactionAlterUpdate>(&transaction_update);
}

namespace {

void RemoveFilesBestEffort(ClientContext &context, FileSystem &fs, const vector<string> &paths,
                           const char *description) {
	if (paths.empty()) {
		return;
	}
	try {
		fs.RemoveFiles(paths);
		DUCKDB_LOG(context, IcebergLogType, "Iceberg Transaction Cleanup, deleted %llu %s(s)",
		           static_cast<uint64_t>(paths.size()), description);
	} catch (std::exception &ex) {
		//! A bulk delete reports one error for the whole batch, so some of these files may well have been removed.
		DUCKDB_LOG(context, IcebergLogType,
		           "Iceberg Transaction Cleanup, failed to delete one or more of %llu %s(s): %s",
		           static_cast<uint64_t>(paths.size()), description, ex.what());
	}
}

} // namespace

void IcebergTransaction::Commit() {
	if (!HasTableUpdate() && created_schemas.empty() && deleted_schemas.empty() && schema_property_updates.empty()) {
		// Read-only transactions have no catalog commit work; temporary vended storage secrets
		// are left to transaction/session cleanup.
		return;
	}

	Connection temp_con(db);
	temp_con.BeginTransaction();
	auto &temp_con_context = temp_con.context;

	// Copy user settings from the original context so that e.g. s3_access_key_id are available
	if (!this->context.expired()) {
		temp_con_context->config = this->context.lock()->config;
	}

	try {
		DoSchemaCreates(*temp_con_context);
		DoSchemaPropertyUpdates(*temp_con_context);
		std::visit(
		    [&](auto &update) {
			    using T = std::decay_t<decltype(update)>;
			    if constexpr (std::is_same_v<T, std::monostate>) {
				    return;
			    } else if constexpr (std::is_same_v<T, IcebergTransactionAlterUpdate>) {
				    DoTableUpdates(update, *temp_con_context);
			    } else if constexpr (std::is_same_v<T, IcebergTransactionDeleteUpdate>) {
				    DoTableDeletes(update, *temp_con_context);
			    } else if constexpr (std::is_same_v<T, IcebergTransactionRenameUpdate>) {
				    DoTableRename(update, *temp_con_context);
			    }
		    },
		    transaction_update);
		DoSchemaDeletes(*temp_con_context);
	} catch (std::exception &ex) {
		ErrorData error(ex);
		commit_state_unknown = catalog.GetBackend().CommitStateUnknown(error);
		CleanupFiles();
		temp_con.Rollback();
		EvictCachedTables();
		error.Throw("Failed to commit Iceberg transaction: ");
	}

	temp_con.Rollback();
}

void IcebergTransaction::DoTableUpdates(IcebergTransactionAlterUpdate &alter_update, ClientContext &context) {
	if (!alter_update.HasUpdates()) {
		return;
	}
	catalog.GetBackend().CommitTableUpdates(context, alter_update);

	for (auto &entry : alter_update.updated_tables) {
		catalog.GetBackend().InvalidateTable(entry.second.get());
	}
}

void IcebergTransaction::DoTableRename(IcebergTransactionRenameUpdate &rename_update, ClientContext &context) {
	auto &original_table = rename_update.table.get();
	auto &schema = original_table.schema;
	auto &table_name = original_table.name;
	auto new_name = rename_update.new_name;
	auto &new_table = rename_update.new_table.get();

	catalog.GetBackend().RenameTable(context, original_table, new_name);

	//! The shared cache must only change once the catalog rename is durable.
	catalog.GetBackend().InvalidateTable(original_table);
	catalog.GetBackend().InvalidateTable(new_table);

	schema.tables.RenameEntry(table_name, new_name, std::move(new_table));
}

void IcebergTransaction::DoTableDeletes(IcebergTransactionDeleteUpdate &delete_update, ClientContext &context) {
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	auto &table = delete_update.deleted_table.get();
	auto &table_name = table.name;
	catalog.GetBackend().DropTable(context, table);
	// remove the load table result
	ic_catalog.GetBackend().InvalidateTable(table);
	// remove the table entry from the catalog
	DropInfo drop_info;
	drop_info.GetQualifiedNameMutable() = Identifier(table_name);
	drop_info.if_not_found = OnEntryNotFound::RETURN_NULL;
	table.schema.DropEntry(context, drop_info, true);
}

void IcebergTransaction::DoSchemaCreates(ClientContext &context) {
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	for (auto &created_schema : created_schemas) {
		auto &schema_name = created_schema.first;
		catalog.GetBackend().CreateSchema(context, created_schema.second->namespace_items);
		ic_catalog.GetSchemas().AddEntry(schema_name, created_schema.second);
	}
}

void IcebergTransaction::DoSchemaDeletes(ClientContext &context) {
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	for (auto &schema_name : deleted_schemas) {
		catalog.GetBackend().DropSchema(context, StringUtil::Split(schema_name, '.'));
		ic_catalog.GetSchemas().RemoveEntry(schema_name);
	}
	deleted_schemas.clear();
}

void IcebergTransaction::DoSchemaPropertyUpdates(ClientContext &context) {
	for (auto &properties_update : this->schema_property_updates) {
		auto schema_name_with_catalog = properties_update.first;
		auto catalog_splitter = schema_name_with_catalog.find(".");
		auto schema_name_no_catalog = schema_name_with_catalog.erase(0, catalog_splitter + 1);

		auto schema_property_updates = properties_update.second;
		catalog.GetBackend().UpdateSchemaProperties(context, StringUtil::Split(schema_name_no_catalog, '.'),
		                                            schema_property_updates.updates, schema_property_updates.removals);
	}
}

namespace {

struct ScopedTransaction {
public:
	ScopedTransaction(DatabaseInstance &db) : connection(db) {
		connection.BeginTransaction();
	}
	~ScopedTransaction() {
		//! Prevent the connection from destructing with an active transaction
		//! As that causes it to ROLLBACK and enter CleanupFiles - resulting in a stack overflow due to recursion
		auto result = connection.Query("COMMIT");
		if (result->HasError()) {
			connection.Query("ROLLBACK");
		}
	}

public:
	ClientContext &GetContext() {
		return *connection.context;
	}

public:
	Connection connection;
};

} // namespace

void IcebergTransaction::CleanupFiles() {
	// remove any files that were written
	if (!catalog.attach_options.remove_files_on_delete) {
		// certain catalogs don't allow deletes and will have a s3.deletes attribute in the config describing this
		// aws s3 tables rejects deletes and will handle garbage collection on its own, any attempt to delete the files
		// on the aws side will result in an error.
		return;
	}
	if (commit_state_unknown) {
		// Commit may have landed (CommitStateUnknownException); keep the files.
		return;
	}
	ScopedTransaction temp_con(db);
	auto &temp_context = temp_con.GetContext();
	auto &fs = FileSystem::GetFileSystem(temp_context);

	if (auto alter_update = GetAlterUpdate()) {
		for (auto &up_table : alter_update->updated_tables) {
			auto &table = up_table.second.get();
			if (!table.transaction_data) {
				// error occurred before transaction data was initialized
				// this can happen during table creation with table schema that cannot convert to
				// an iceberg table schema due to type incompatabilities
				continue;
			}
			auto &transaction_data = table.transaction_data;
			//! Batched per table: the keys PrepareIcebergScanFromEntry registers below belong to this table, so
			//! the files have to be deleted before moving on to the next one.
			vector<string> data_files;
			for (auto &update : transaction_data->updates) {
				if (update->type != IcebergTableUpdateType::ADD_SNAPSHOT) {
					continue;
				}
				// we need to recreate the keys in the current context.
				auto &ic_table_entry = table.GetLatestSchema()->Cast<IcebergTableSchemaVersion>();
				ic_table_entry.PrepareIcebergScanFromEntry(temp_context);

				auto &add_snapshot = update->Cast<IcebergAddSnapshot>();
				const auto manifest_list_entries = add_snapshot.GetManifestFiles();
				for (const auto &manifest : manifest_list_entries) {
					for (auto &manifest_entry : manifest.GetManifestEntries()) {
						data_files.push_back(manifest_entry.data_file.file_path);
					}
				}
			}
			RemoveFilesBestEffort(temp_context, fs, data_files, "data_file");
		}
	}
}

void IcebergTransaction::EvictCachedTables() {
	std::visit(
	    [&](auto &update) {
		    using T = std::decay_t<decltype(update)>;
		    if constexpr (std::is_same_v<T, IcebergTransactionAlterUpdate>) {
			    for (auto &up_table : update.updated_tables) {
				    catalog.GetBackend().InvalidateTable(up_table.second.get());
			    }
		    } else if constexpr (std::is_same_v<T, IcebergTransactionDeleteUpdate>) {
			    catalog.GetBackend().InvalidateTable(update.deleted_table.get());
		    }
	    },
	    transaction_update);
}

void IcebergTransaction::Rollback() {
	CleanupFiles();
}

IcebergTransaction &IcebergTransaction::Get(ClientContext &context, Catalog &catalog) {
	D_ASSERT(catalog.GetCatalogType() == "iceberg");
	return Transaction::Get(context, catalog).Cast<IcebergTransaction>();
}

bool IcebergTransaction::StartedBefore(timestamp_ms_t timestamp_ms) const {
	auto ctx = context.lock();
	auto transaction_start_ms = IcebergUtils::GetTransactionStartTimeMS(*ctx);
	return transaction_start_ms < timestamp_ms;
}

optional_ptr<IcebergTransactionTableState> IcebergTransaction::GetLatestTableState(const string &table_key) {
	auto it = current_table_data.find(table_key);
	if (it == current_table_data.end()) {
		return nullptr;
	}
	return it->second;
}

IcebergTransactionTableState &IcebergTransaction::SetLatestTableState(const string &table_key,
                                                                      IcebergTableStatus status) {
	auto it = current_table_data.find(table_key);
	if (it == current_table_data.end()) {
		it = current_table_data.emplace(table_key, IcebergTransactionTableState()).first;
	}
	it->second.SetStatus(status);
	return it->second;
}

IcebergTransactionTableState &IcebergTransaction::SetCatalogTableState(shared_ptr<IcebergTable> table) {
	auto table_key = table->GetTableKey();
	auto result = current_table_data.emplace(table_key, IcebergTransactionTableState(std::move(table)));
	return result.first->second;
}

IcebergTransactionTableState &
IcebergTransaction::SetTransactionTableState(const string &table_key, IcebergTable &&table, IcebergTableStatus status) {
	auto it = current_table_data.find(table_key);
	if (it == current_table_data.end()) {
		it = current_table_data.emplace(table_key, IcebergTransactionTableState(std::move(table))).first;
	} else {
		if (!it->second.IsMissing()) {
			throw InternalException("Transaction state already exists for table '%s'", table_key);
		}
		it->second = IcebergTransactionTableState(std::move(table));
	}
	it->second.SetStatus(status);
	return it->second;
}

IcebergTransactionTableState &IcebergTransaction::GetOrCreateTransactionTableState(const IcebergTable &table) {
	auto table_key = table.GetTableKey();
	auto state = GetLatestTableState(table_key);
	if (state) {
		return *state;
	}
	auto copy = table.Copy(*this);
	return SetTransactionTableState(table_key, std::move(copy), IcebergTableStatus::ALIVE);
}

IcebergTransactionAlterUpdate &IcebergTransaction::GetOrCreateAlter() {
	if (!HasTableUpdate()) {
		transaction_update.emplace<IcebergTransactionAlterUpdate>(*this);
	}
	auto alter_update = GetAlterUpdate();
	if (!alter_update) {
		throw TransactionException("%s cannot commit this transaction atomically because it mixes "
		                           "table updates with rename/drop requests",
		                           catalog.GetBackend().GetDescription());
	}
	return *alter_update;
}

IcebergTable &IcebergTransaction::DeleteTable(IcebergTable &table) {
	auto table_key = table.GetTableKey();
	auto state = GetLatestTableState(table_key);
	if (HasTableUpdate()) {
		throw TransactionException("%s cannot commit this transaction atomically because it mixes "
		                           "table updates with rename/drop requests",
		                           catalog.GetBackend().GetDescription());
	}

	if (!state) {
		state = GetOrCreateTransactionTableState(table);
	}
	auto &deleted_table = state->GetOrCreateTransactionInfo(*this);
	state->SetStatus(IcebergTableStatus::DROPPED);
	transaction_update.emplace<IcebergTransactionDeleteUpdate>(*this, deleted_table);
	return state->GetInfo();
}

IcebergTable &IcebergTransaction::RenameTable(IcebergTable &table, const string &new_name) {
	auto table_key = table.GetTableKey();
	auto state = GetLatestTableState(table_key);
	if (HasTableUpdate()) {
		throw TransactionException("%s cannot commit this transaction atomically because it mixes "
		                           "table updates with rename/drop requests",
		                           catalog.GetBackend().GetDescription());
	}

	if (!state) {
		state = GetOrCreateTransactionTableState(table);
	}
	state->SetStatus(IcebergTableStatus::RENAMED);
	auto &source_table = state->GetInfo();
	auto new_table = source_table.Copy();
	new_table.name = new_name;
	auto new_table_key = new_table.GetTableKey();
	auto &new_state = SetTransactionTableState(new_table_key, std::move(new_table), IcebergTableStatus::ALIVE);

	//! Create the rename update, creating the new IcebergTable in the process
	transaction_update.emplace<IcebergTransactionRenameUpdate>(*this, source_table, new_state.GetInfo(), new_name);
	return state->GetInfo();
}

void ApplyTableUpdate(IcebergTable &table_info, IcebergTransaction &iceberg_transaction,
                      const std::function<void(IcebergTable &)> &callback) {
	auto &alter = iceberg_transaction.GetOrCreateAlter();
	auto &updated_table = alter.GetOrInitializeTable(table_info);
	callback(updated_table);
}

} // namespace duckdb
