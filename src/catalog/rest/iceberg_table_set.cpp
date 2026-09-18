#include "catalog/rest/iceberg_table_set.hpp"

#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/planner/parsed_data/bound_create_table_info.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "duckdb/parser/parser.hpp"
#include "duckdb/planner/tableref/bound_at_clause.hpp"
#include "duckdb/planner/expression_binder/table_function_binder.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/logging/logger.hpp"

#include "catalog/iceberg_catalog_backend.hpp"
#include "catalog/iceberg_create_table_info.hpp"
#include "catalog/rest/api/iceberg_create_table_request.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table_schema_version.hpp"
#include "catalog/rest/transaction/iceberg_transaction.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "core/metadata/partition/iceberg_partition_spec.hpp"
#include "catalog/rest/transaction/iceberg_transaction_update.hpp"
#include "iceberg_options.hpp"

namespace duckdb {

IcebergTableSet::IcebergTableSet(IcebergSchemaEntry &schema) : schema(schema), catalog(schema.ParentCatalog()) {
}

bool IcebergTableSet::FillEntry(ClientContext &context, IcebergTable &table) {
	// If the table is already loaded, no need to fill again
	if (!table.schema_versions.empty()) {
		return true;
	}

	return catalog.Cast<IcebergCatalog>().GetBackend().LoadTable(context, table);
}

IcebergTableSchemaVersion &IcebergTableSet::GetOrCreateDummy(IcebergTable &table_info) const {
	if (table_info.dummy_entry) {
		return *table_info.dummy_entry;
	}
	// create a table entry with fake schema data to avoid calling the LoadTableInformation endpoint for every
	// table while listing schemas
	CreateTableInfo info(schema, Identifier(table_info.name));
	vector<ColumnDefinition> columns;
	auto col = ColumnDefinition(Identifier("__"), LogicalType::UNKNOWN);
	columns.push_back(std::move(col));
	info.columns = ColumnList(std::move(columns));
	auto table_entry = make_uniq<IcebergTableSchemaVersion>(table_info, catalog, schema, info, optional_idx());
	if (!table_entry->internal) {
		table_entry->internal = schema.internal;
	}
	auto result = table_entry.get();
	if (result->name.empty()) {
		throw InternalException("IcebergTableSet::CreateEntry called with empty name");
	}
	table_info.dummy_entry = std::move(table_entry);
	return *table_info.dummy_entry;
}

void IcebergTableSet::Scan(ClientContext &context, const std::function<void(CatalogEntry &)> &callback) {
	annotated_lock_guard<annotated_mutex> lock(entry_lock);
	auto &iceberg_transaction = IcebergTransaction::Get(context, catalog);
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	LoadEntriesInternal(context);
	const bool eager = ic_catalog.attach_options.table_resolution == IcebergTableResolution::EAGER;
	for (auto &entry : entries) {
		auto &table_info = *entry.second;
		auto table_key = table_info.GetTableKey();
		iceberg_transaction.tables[table_key] = entry.second;

		if (eager && table_info.schema_versions.empty()) {
			try {
				FillEntry(context, table_info);
			} catch (std::exception &ex) {
				ErrorData error(ex);
				DUCKDB_LOG_WARNING(context, "Could not resolve the columns of Iceberg table '%s' while listing: %s",
				                   table_key, error.RawMessage());
			}
		}

		if (!table_info.schema_versions.empty()) {
			// The table has already been resolved (e.g. via DESCRIBE or a scan), so its full schema -
			// including column comments mapped from the Iceberg field 'doc' - is available. Surface the
			// resolved entry instead of the placeholder so listings reflect the real columns.
			auto resolved = table_info.GetLatestSchema();
			if (resolved) {
				callback(*resolved);
				continue;
			}
		}

		auto &dummy = GetOrCreateDummy(table_info);
		callback(dummy);
	}
}

void IcebergTableSet::ScanTables(ClientContext &context, const std::function<void(IcebergTable &)> &callback) {
	annotated_lock_guard<annotated_mutex> lock(entry_lock);
	LoadEntriesInternal(context);
	for (auto &entry : entries) {
		callback(*entry.second);
	}
}

void IcebergTableSet::DropEntry(ClientContext &context, DropInfo &info, bool delete_entry) {
	annotated_lock_guard<annotated_mutex> lock(entry_lock);
	auto table_name = info.GetQualifiedName().Name();
	auto entry = entries.find(table_name.GetIdentifierName());
	if (entry == entries.end()) {
		if (info.if_not_found == OnEntryNotFound::RETURN_NULL) {
			return;
		}
		throw CatalogException("Table %s does not exist", table_name);
	}
	if (info.cascade) {
		throw NotImplementedException("DROP TABLE <table_name> CASCADE is not supported for Iceberg tables currently");
	}
	if (delete_entry) {
		entries.erase(entry);
		return;
	}

	// Add the table to the transaction's deleted tables.
	auto &transaction = IcebergTransaction::Get(context, catalog).Cast<IcebergTransaction>();
	auto &table = transaction.DeleteTable(*entry->second);
	//! FIXME: Schema versions point back to their IcebergTable and must be reinitialized after the copy.
	table.InitSchemaVersions();
}

void IcebergTableSet::RenameEntry(const string &name, const string &new_name, IcebergTable &&new_table) {
	annotated_lock_guard<annotated_mutex> lock(entry_lock);
	auto source = entries.find(name);
	if (source == entries.end()) {
		throw CatalogException("Table %s does not exist", name);
	}
	entries.erase(source);
	shared_ptr<IcebergTable> old_version;
	CreateEntryInternal(new_name, std::move(new_table), old_version);
	if (old_version) {
		throw TransactionException("Table %s was already created by a different transaction!", new_name);
	}
}

void IcebergTableSet::LoadEntriesInternal(ClientContext &context) {
	auto &iceberg_transaction = IcebergTransaction::Get(context, catalog);
	bool schema_listed = iceberg_transaction.listed_schemas.find(schema.name.GetIdentifierName()) !=
	                     iceberg_transaction.listed_schemas.end();
	if (schema_listed) {
		return;
	}
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	auto tables = ic_catalog.GetBackend().ListTables(context, schema);
	// A refused listing says nothing about which tables exist, so the cache is left untouched.
	if (tables) {
		case_insensitive_set_t listed;
		for (auto &table : *tables) {
			listed.insert(table);
			entries.emplace(table, IcebergTable::CreatePlaceholder(ic_catalog, schema, table));
		}
		// 'entries' outlives the transaction, so drop the names the listing no longer reports.
		// Tables created in this transaction live on the transaction, not here, so they are safe.
		for (auto it = entries.begin(); it != entries.end();) {
			if (listed.find(it->first) == listed.end()) {
				it = entries.erase(it);
			} else {
				++it;
			}
		}
	}
	iceberg_transaction.listed_schemas.insert(schema.name.GetIdentifierName());
}

static Value ParseTableProperty(TableFunctionBinder &binder, ClientContext &context, const ParsedExpression &expr_ref,
                                const string &property_name, const LogicalType &type) {
	auto expr = expr_ref.Copy();
	auto bound_expr = binder.Bind(expr);
	if (bound_expr->HasParameter()) {
		throw ParameterNotResolvedException();
	}

	auto val = ExpressionExecutor::EvaluateScalar(context, *bound_expr, true);
	if (val.IsNull()) {
		throw BinderException("NULL is not supported as a valid option for '%s'", property_name);
	}
	auto casted_val = val.DefaultTryCastAs(type, nullptr, true);
	if (!casted_val) {
		throw InvalidInputException("Can't cast '%s' property (%s) to %s", property_name, val.ToString(),
		                            type.ToString());
	}
	return std::move(*casted_val);
}

shared_ptr<IcebergTable> IcebergTableSet::CreateEntryInternal(const string &name, IcebergTable &&table,
                                                              shared_ptr<IcebergTable> &old_entry) {
	auto it = entries.find(name);
	if (it != entries.end()) {
		old_entry = std::move(it->second);
		it->second = make_shared_ptr<IcebergTable>(std::move(table));
	} else {
		it = entries.emplace(name, make_shared_ptr<IcebergTable>(std::move(table))).first;
	}
	return it->second;
}

IcebergTable &IcebergTableSet::CreateNewEntry(ClientContext &context, IcebergCatalog &catalog,
                                              IcebergSchemaEntry &schema, CreateTableInfo &info) {
	auto &iceberg_transaction = IcebergTransaction::Get(context, catalog);

	auto binder = Binder::CreateBinder(context);
	TableFunctionBinder property_binder(*binder, context, "format-version");

	optional_idx iceberg_version;
	case_insensitive_map_t<Value> table_properties;
	// format version must be verified
	auto format_version_it = info.options.find("format-version");
	if (format_version_it != info.options.end()) {
		iceberg_version = ParseTableProperty(property_binder, context, *format_version_it->second, "format-version",
		                                     LogicalType::INTEGER)
		                      .GetValue<int32_t>();
		if (iceberg_version.GetIndex() < 1) {
			throw InvalidInputException("The lowest supported iceberg version is 1!");
		}
	} else {
		Value default_version_value;
		if (context.TryGetCurrentSetting(DEFAULT_FORMAT_VERSION_CONFIG_VARIABLE, default_version_value)) {
			iceberg_version = default_version_value.GetValue<uint64_t>();
		} else {
			iceberg_version = DEFAULT_ICEBERG_FORMAT_VERSION;
		}
	}

	string location;
	auto location_it = info.options.find("location");
	if (location_it != info.options.end()) {
		location = ParseTableProperty(property_binder, context, *location_it->second, "location", LogicalType::VARCHAR)
		               .GetValue<string>();
	}
	if (location.empty() && catalog.attach_options.default_table_location_from_namespace) {
		schema.LoadProperties(context);
		auto ns_location_it = schema.schema_info.properties.find("location");
		if (ns_location_it != schema.schema_info.properties.end() && !ns_location_it->second.empty()) {
			location = ns_location_it->second;
			StringUtil::RTrim(location, "/");
			location += "/" + info.GetTableName().GetIdentifierName();
		}
	}

	IcebergTableMetadata bootstrap_metadata(IcebergTableMetadataSchemas {});
	bootstrap_metadata.iceberg_version = iceberg_version.GetIndex();
	int32_t last_column_id;

	auto new_schema = IcebergCreateTableRequest::CreateIcebergSchema(context, bootstrap_metadata, info.columns,
	                                                                 &info.constraints, last_column_id);
	new_schema->schema_id = 0;
	bootstrap_metadata.last_column_id = last_column_id;
	bootstrap_metadata.SetCurrentSchemaId(0);

	// Get Location
	if (!location.empty()) {
		bootstrap_metadata.location = location;
	}
	for (auto &option : info.options) {
		if (option.first == "format-version" || option.first == "location") {
			continue;
		}
		auto option_val =
		    ParseTableProperty(property_binder, context, *option.second, option.first, LogicalType::VARCHAR)
		        .GetValue<string>();
		bootstrap_metadata.table_properties.emplace(option.first, option_val);
	}

	auto initial_partition_spec = IcebergTable::BuildPartitionSpec(info.partition_keys, *new_schema, 0, 1000);
	IcebergCreateTableInfo create_table_info {info.GetTableName().GetIdentifierName(), new_schema,
	                                          std::move(initial_partition_spec),       iceberg_version.GetIndex(),
	                                          bootstrap_metadata.table_properties,     bootstrap_metadata.location};

	// Let the backend initialize the metadata, locations and transaction state for the new table.
	auto table = IcebergTable(catalog, schema, info.GetTableName().GetIdentifierName(),
	                          IcebergTableMetadata(IcebergTableMetadataSchemas {}));
	return catalog.GetBackend().CreateTable(context, iceberg_transaction, std::move(table), create_table_info);
}

optional_ptr<CatalogEntry> IcebergTableSet::GetEntry(ClientContext &context, const EntryLookupInfo &lookup) {
	auto &ic_catalog = catalog.Cast<IcebergCatalog>();
	auto &iceberg_transaction = IcebergTransaction::Get(context, catalog);
	const auto &table_name = lookup.GetEntryName();
	// first check transaction entries
	const auto table_key = IcebergTable::GetTableKey(ic_catalog, schema.namespace_items, table_name);
	auto latest_state = iceberg_transaction.GetLatestTableState(table_key);

	auto at = lookup.GetAtClause();
	if (latest_state) {
		if (!latest_state->IsAlive()) {
			// If table has been deleted or is missing within the transaction, return null
			return nullptr;
		}
		auto &table_info = latest_state->GetInfo();
		if (table_info.schema_versions.empty()) {
			table_info.InitSchemaVersions();
		}
		return table_info.GetSchemaVersion(at);
	}

	auto new_version = IcebergTable::CreatePlaceholder(ic_catalog, schema, table_name);
	auto &table_info = *new_version;
	if (!FillEntry(context, table_info)) {
		//! The table doesn't exist in the catalog
		iceberg_transaction.SetLatestTableState(table_key, IcebergTableStatus::MISSING);
		return nullptr;
	}

	{
		annotated_lock_guard<annotated_mutex> l(entry_lock);
		entries[table_name] = new_version;
	}
	iceberg_transaction.tables[table_key] = new_version;
	auto &state = iceberg_transaction.SetCatalogTableState(new_version);
	if (iceberg_transaction.StartedBefore(table_info.table_metadata.last_updated_ms)) {
		state.GetOrCreateTransactionInfo(iceberg_transaction);
	}
	return state.GetInfo().GetSchemaVersion(at);
}

} // namespace duckdb
