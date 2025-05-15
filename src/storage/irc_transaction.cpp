#include "storage/irc_transaction.hpp"
#include "storage/irc_catalog.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "duckdb/catalog/catalog_entry/index_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/view_catalog_entry.hpp"
#include "storage/irc_schema_entry.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/catalog/entry_lookup_info.hpp"
#include "duckdb/common/error_data.hpp"

namespace duckdb {

IcebergSchemaInformation::IcebergSchemaInformation(const string &table_name, rest_api_objects::Schema &schema) {
	CreateTableInfo info;
	info.table = table_name;

	for (auto &field : schema.struct_type.fields) {
		auto column_def = IcebergColumnDefinition::ParseStructField(*field);

		info.columns.AddColumn(ColumnDefinition(column_def->name, column_def->type));
		columns.push_back(std::move(column_def));
	}
}

IcebergTableInformation::IcebergTableInformation(Catalog &catalog, IRCSchemaEntry &schema, const string &name)
    : schema(schema), name(name) {
}

int64_t IcebergTableInformation::SnapshotFromTimestamp(timestamp_t timestamp) {
	auto timestamp_millis = Timestamp::GetEpochMs(timestamp);

	uint64_t max_millis = NumericLimits<uint64_t>::Minimum();
	int64_t snapshot_id;
	//! Find the last entry that is equal to timestamp or is older than timestamp
	for (auto &entry : timestamp_to_snapshot) {
		if (entry.first <= timestamp_millis && entry.first >= max_millis) {
			snapshot_id = entry.second;
			max_millis = entry.first;
		}
	}
	if (max_millis == NumericLimits<uint64_t>::Minimum()) {
		//! There either were no snapshots in the log, or they are all newer than 'timestamp'
		throw InvalidInputException("No snapshots exist or all snapshot were made after '%s'",
		                            Timestamp::ToString(timestamp));
	}
	return snapshot_id;
}

int64_t IcebergTableInformation::GetSnapshot(const BoundAtClause &at) {
	D_ASSERT(has_metadata);

	auto &unit = at.Unit();
	auto &value = at.GetValue();

	int64_t snapshot_id;
	if (StringUtil::CIEquals(unit, "version")) {
		if (value.type().id() != LogicalTypeId::BIGINT) {
			throw InvalidInputException("'version' has to be provided as a BIGINT value");
		}
		snapshot_id = value.GetValue<int64_t>();
	} else if (StringUtil::CIEquals(unit, "timestamp")) {
		if (value.type().id() != LogicalTypeId::TIMESTAMP) {
			throw InvalidInputException("'timestamp' has to be provided as a TIMESTAMP value");
		}
		snapshot_id = SnapshotFromTimestamp(value.GetValue<timestamp_t>());
	} else {
		throw InvalidInputException(
		    "Unit '%s' for time travel is not valid, supported options are 'version' and 'timestamp'", unit);
	}
	return snapshot_id;
}

IRCNamespaceInformation::IRCNamespaceInformation(Catalog &catalog, const string &name) {
	CreateSchemaInfo create_info;
	create_info.schema = name;
	catalog_entry = make_uniq<IRCSchemaEntry>(catalog, create_info, *this);
}

IRCTransaction::IRCTransaction(IRCatalog &ic_catalog, TransactionManager &manager, ClientContext &context)
    : Transaction(manager, context), catalog(ic_catalog), access_mode(ic_catalog.access_mode) {
}

IRCTransaction::~IRCTransaction() = default;

void IRCTransaction::Start() {
	transaction_state = IRCTransactionState::TRANSACTION_NOT_YET_STARTED;
}
void IRCTransaction::Commit() {
	if (transaction_state == IRCTransactionState::TRANSACTION_STARTED) {
		transaction_state = IRCTransactionState::TRANSACTION_FINISHED;
	}
}
void IRCTransaction::Rollback() {
	if (transaction_state == IRCTransactionState::TRANSACTION_STARTED) {
		transaction_state = IRCTransactionState::TRANSACTION_FINISHED;
	}
}

IRCTransaction &IRCTransaction::Get(ClientContext &context, Catalog &catalog) {
	return Transaction::Get(context, catalog).Cast<IRCTransaction>();
}

optional_ptr<IRCSchemaEntry> IRCTransaction::GetSchema(ClientContext &context, const string &schema) {
	auto namespace_info = namespaces.find(schema);
	if (namespace_info != namespaces.end()) {
		return namespace_info->second.catalog_entry.get();
	}

	auto it = namespaces.emplace(schema, IRCNamespaceInformation(catalog, schema));
	return it.second.catalog_entry.get();
}

optional_ptr<ICTableEntry> IRCTransaction::GetTable(ClientContext &context, IRCSchemaEntry &schema,
                                                    const EntryLookupInfo &lookup) {
	auto &namespace_info = schema.namespace_info;
	auto cached_result = namespace_info.table_metadata.find(name);
	auto at = lookup.GetAtClause();
	if (cached_result != namespace_info.table_metadata.end()) {
		auto &table_info = cached_result.second;
		auto &metadata = table_info.metadata.metadata;
		if (at) {
			//! Specific snapshot information is attached, look up the corresponding schema
			auto schema_id = table_info.GetSnapshot(*at);
			auto it = table_info.table_schemas.find(schema_id);
			D_ASSERT(it != table_info.table_schemas.end());
			auto &schema_info = it.second;
			return schema_info.table_entry.get();
		} else {
			//! Since this is cached, we're assuming we already have the latest version
			//! Just return the last snapshot of the table
			D_ASSERT(metadata.has_current_schema_id);
			auto schema_id = metadata.current_schema_id;
			auto it = table_info.table_schemas.find(schema_id);
			D_ASSERT(it != table_info.table_schemas.end());
			auto &schema_info = it.second;
			return schema_info.table_entry.get();
		}
	}

	auto table_name = lookup.GetEntryName();
	IcebergTableInformation new_table_info(catalog, schema, table_name);

	//! FIXME: we want to get the raw response back, with error status, instead of try/catching
	try {
		new_table_info.metadata = IRCAPI::GetTable(context, catalog, schema, table_name);
		new_table_info.has_metadata = true;

		auto &metadata = new_table_info.metadata.metadata;
		auto &timestamp_to_snapshot = new_table_info.timestamp_to_snapshot;
		auto &snapshot_log = metadata.snapshot_log;
		for (auto &entry : snapshot_log.value) {
			timestamp_to_snapshot[entry.timestamp_ms] = entry.snapshot_id;
		}

		auto &table_snapshots = new_table_info.table_snapshots;
		auto &snapshots = metadata.snapshots;
		for (auto &entry : snapshots) {
			auto snapshot_id = entry.snapshot_id;
			table_snapshots.emplace(snapshot_id, std::move(entry));
		}

		auto &table_schemas = new_table_info.table_schemas;
		auto &schemas = metadata.schemas;
		for (auto &entry : schemas) {
			D_ASSERT(entry.object_1.has_schema_id);
			auto schema_id = entry.object_1.schema_id;
			table_schemas.emplace(schema_id, IcebergSchemaInformation(table_name, entry));
		}
	} catch (std::exception &ex) {
		error = ErrorData(ex);
		if (error.Type() == ExceptionType::INVALID_CONFIGURATION || error.Type() == ExceptionType::INVALID_INPUT) {
			//! Assume it doesn't exist if an error occurred.
			return nullptr;
		}
		throw;
	}
	auto it = namespace_info.table_metadata.emplace(table_name, std::move(new_table_info));
	auto &table_info = it.second;
	if (at) {
		auto schema_id = table_info.GetSnapshot(*at);
		auto it = table_info.table_schemas.find(schema_id);
		D_ASSERT(it != table_info.table_schemas.end());
		auto &schema_info = it.second;
		return schema_info.table_entry.get();
	} else {
		auto &metadata = table_info.metadata.metadata;
		D_ASSERT(metadata.has_current_schema_id);
		auto schema_id = metadata.current_schema_id;
		auto it = table_info.table_schemas.find(schema_id);
		D_ASSERT(it != table_info.table_schemas.end());
		auto &schema_info = it.second;
		return schema_info.table_entry.get();
	}
}

} // namespace duckdb
