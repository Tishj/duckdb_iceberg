#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/iceberg_catalog_backend.hpp"
#include "duckdb/storage/database_size.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"
#include "duckdb/planner/operator/logical_create_table.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/transaction/iceberg_transaction.hpp"

namespace duckdb {

IcebergCatalog::IcebergCatalog(AttachedDatabase &db_p, AccessMode access_mode,
                               unique_ptr<IcebergCatalogBackend> backend, IcebergAttachOptions &attach_options_p,
                               const Identifier &default_schema)
    : Catalog(db_p), access_mode(access_mode), attach_options(attach_options_p), default_schema(default_schema),
      backend(std::move(backend)), schemas(*this) {
}

IcebergCatalog::~IcebergCatalog() = default;

//===--------------------------------------------------------------------===//
// Catalog API
//===--------------------------------------------------------------------===//

IcebergSchemaSet &IcebergCatalog::GetSchemas() {
	return schemas;
}

void IcebergCatalog::Initialize(bool load_builtin) {
}

void IcebergCatalog::ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) {
	schemas.Scan(context, [&](CatalogEntry &schema) { callback(schema.Cast<IcebergSchemaEntry>()); });
}

optional_ptr<SchemaCatalogEntry> IcebergCatalog::LookupSchema(CatalogTransaction transaction,
                                                              const EntryLookupInfo &schema_lookup,
                                                              OnEntryNotFound if_not_found) {
	auto &schema_name = schema_lookup.GetEntryName();
	auto entry = schemas.GetEntry(transaction.GetContext(), schema_name, if_not_found);
	if (!entry && if_not_found != OnEntryNotFound::RETURN_NULL) {
		throw CatalogException(schema_lookup.GetErrorContext(), "Schema with name \"%s\" not found", schema_name);
	}

	return reinterpret_cast<SchemaCatalogEntry *>(entry.get());
}

optional<Identifier> IcebergCatalog::GetDefaultSchema() const {
	if (default_schema.empty()) {
		return nullopt;
	}
	return default_schema;
}

optional_ptr<CatalogEntry> IcebergCatalog::CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) {
	optional_ptr<ClientContext> context = transaction.GetContext();
	if (info.on_conflict == OnCreateConflict::REPLACE_ON_CONFLICT) {
		throw NotImplementedException(
		    "CREATE OR REPLACE not supported in DuckDB-Iceberg. Please use separate Drop and Create Statements");
	}

	D_ASSERT(context);
	auto &iceberg_transaction = IcebergTransaction::Get(*context, *this);
	auto &schema_name = info.GetQualifiedName().Schema().GetIdentifierName();
	auto created_schema = iceberg_transaction.created_schemas.find(schema_name);
	if (created_schema != iceberg_transaction.created_schemas.end()) {
		if (info.on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
			return created_schema->second.get();
		}
		throw CatalogException("Schema with name \"%s\" already exists", info.GetQualifiedName().Schema());
	}

	// Verify schema existence on the server first
	bool schema_exists = GetBackend().SchemaExists(*context, schema_name);

	if (schema_exists) {
		if (info.on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
			// Schema already exists on the server - get or create a local entry and return it
			auto entry = schemas.GetEntry(*context, schema_name, OnEntryNotFound::RETURN_NULL);
			if (entry) {
				return entry;
			}
			auto new_schema = make_shared_ptr<IcebergSchemaEntry>(*this, info);
			schemas.AddEntry(schema_name, new_schema);
			iceberg_transaction.schemas[schema_name] = new_schema;
			return new_schema.get();
		}
		throw CatalogException("Schema with name \"%s\" already exists", info.GetQualifiedName().Schema());
	}

	// Schema does not exist - stage it locally and defer the server creation and catalog publication to commit
	auto new_schema = make_shared_ptr<IcebergSchemaEntry>(*this, info);
	auto result = new_schema.get();
	iceberg_transaction.created_schemas.emplace(schema_name, std::move(new_schema));
	return result;
}

void IcebergCatalog::DropSchema(ClientContext &context, DropInfo &info) {
	if (info.cascade) {
		throw NotImplementedException(
		    "DROP SCHEMA <schema_name> CASCADE is not supported for Iceberg schemas currently");
	}

	// Verify schema existence on the server first
	bool schema_exists = GetBackend().SchemaExists(context, info.GetQualifiedName().Name().GetIdentifierName());

	if (!schema_exists) {
		if (info.if_not_found == OnEntryNotFound::RETURN_NULL) {
			// remove the entry if it exists locally
			// it could have been created during the bind phase.
			GetSchemas().RemoveEntry(info.GetQualifiedName().Name().GetIdentifierName());
			return;
		}
		throw CatalogException("Schema with name \"%s\" does not exist", info.GetQualifiedName().Name());
	}

	// Schema exists - defer the server deletion to commit
	auto &iceberg_transaction = IcebergTransaction::Get(context, *this);
	iceberg_transaction.deleted_schemas.insert(info.GetQualifiedName().Name().GetIdentifierName());
}

unique_ptr<LogicalOperator> IcebergCatalog::BindCreateIndex(Binder &binder, CreateStatement &stmt,
                                                            TableCatalogEntry &table,
                                                            unique_ptr<LogicalOperator> plan) {
	throw NotImplementedException("IcebergCatalog BindCreateIndex");
}

bool IcebergCatalog::InMemory() {
	return false;
}

string IcebergCatalog::GetDBPath() {
	return backend->GetDBPath();
}

DatabaseSize IcebergCatalog::GetDatabaseSize(ClientContext &context) {
	DatabaseSize size;
	return size;
}

ErrorData IcebergCatalog::SupportsCreateTable(BoundCreateTableInfo &info) {
	auto &base = info.Base().Cast<CreateTableInfo>();
	if (!base.sort_keys.empty()) {
		return ErrorData(ExceptionType::CATALOG,
		                 StringUtil::Format("SORTED BY is not supported for tables in a %s catalog", GetCatalogType()));
	}
	return ErrorData();
}

//===--------------------------------------------------------------------===//
// Iceberg REST Catalog
//===--------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
// Attach
//===--------------------------------------------------------------------===//

//! Streamlined initialization for recognized catalog types

void IcebergCatalog::SetAttachOptions(const unordered_map<string, Value> &options) {
	normalized_attach_options = NormalizeIcebergAttachOptions(options);
}

bool IcebergCatalog::HasConflictingAttachOptions(const string &path, const AttachOptions &options) {
	//! If the base catalog already considers the path or catalog type to conflict, re-attach.
	if (Catalog::HasConflictingAttachOptions(path, options)) {
		return true;
	}
	//! Otherwise compare the iceberg-specific attach options (URI, credentials, MAX_TABLE_STALENESS, ...)
	//! so that ATTACH OR REPLACE re-runs Attach when any of them changes.
	auto normalized_options = NormalizeIcebergAttachOptions(options.options);
	if (normalized_options.size() != normalized_attach_options.size()) {
		return true;
	}
	for (auto &entry : normalized_options) {
		auto it = normalized_attach_options.find(entry.first);
		if (it == normalized_attach_options.end()) {
			return true;
		}
		if (it->second.type() != entry.second.type() || it->second.ToString() != entry.second.ToString()) {
			return true;
		}
	}
	return false;
}

string IcebergCatalog::GetOnlyMergeOnReadSupportedErrorMessage(const string &table_name, const string &property,
                                                               const string &property_value) {
	return StringUtil::Format("DuckDB-Iceberg only supports merge-on-read for updates/deletes. Table Property '%s' is "
	                          "set to '%s' for table %s"
	                          "You can modify Iceberg table properties wth the set_iceberg_table_properties() "
	                          "function, and remove them with the remove_iceberg_table_properties() function. "
	                          "You can view Iceberg table properties with the iceberg_table_properties() function",
	                          property, property_value, table_name);
}

} // namespace duckdb
