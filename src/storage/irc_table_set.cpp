//#include "catalog_api.hpp"
//#include "catalog_utils.hpp"

//#include "storage/irc_catalog.hpp"
//#include "storage/irc_table_set.hpp"
//#include "storage/irc_transaction.hpp"
//#include "duckdb/parser/constraints/not_null_constraint.hpp"
//#include "duckdb/parser/constraints/unique_constraint.hpp"
//#include "duckdb/parser/expression/constant_expression.hpp"
//#include "duckdb/planner/parsed_data/bound_create_table_info.hpp"
//#include "duckdb/parser/parsed_data/drop_info.hpp"
//#include "duckdb/catalog/dependency_list.hpp"
//#include "duckdb/parser/parsed_data/create_table_info.hpp"
//#include "duckdb/parser/constraints/list.hpp"
//#include "storage/irc_schema_entry.hpp"
//#include "duckdb/parser/parser.hpp"

// namespace duckdb {

// ICTableSet::ICTableSet(IRCSchemaEntry &schema) : ICInSchemaSet(schema) {
//}

// static ColumnDefinition CreateColumnDefinition(ClientContext &context, IcebergColumnDefinition &coldef) {
//	return {coldef.name, coldef.type};
//}

// void ICTableSet::FillEntry(ClientContext &context, unique_ptr<CatalogEntry> &entry) {
//	auto *derived = static_cast<ICTableEntry *>(entry.get());
//	if (!derived->table_data->storage_location.empty()) {
//		return;
//	}

//	auto &ic_catalog = catalog.Cast<IRCatalog>();
//	auto table = IRCAPI::GetTable(context, ic_catalog, schema.name, entry->name, true);
//	entry = _CreateCatalogEntry(context, std::move(table));
//}

// void ICTableSet::LoadEntries(ClientContext &context) {
//	if (!entries.empty()) {
//		return;
//	}

//	auto &ic_catalog = catalog.Cast<IRCatalog>();
//	// TODO: handle out-of-order columns using position property
//	auto tables = IRCAPI::GetTables(context, ic_catalog, schema.name);

//	for (auto &table : tables) {
//		auto entry = _CreateCatalogEntry(context, std::move(table));
//		CreateEntry(std::move(entry));
//	}
//}

// optional_ptr<CatalogEntry> ICTableSet::CreateTable(ClientContext &context, BoundCreateTableInfo &info) {
//	auto &ic_catalog = catalog.Cast<IRCatalog>();
//	auto *table_info = dynamic_cast<CreateTableInfo *>(info.base.get());
//	auto table = IRCAPI::CreateTable(context, ic_catalog, schema.name, table_info);
//	auto entry = _CreateCatalogEntry(context, std::move(table));
//	return CreateEntry(std::move(entry));
//}

//} // namespace duckdb
