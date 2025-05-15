#include "storage/irc_schema_entry.hpp"
#include "storage/irc_table_entry.hpp"
#include "storage/irc_transaction.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "duckdb/parser/parsed_data/create_index_info.hpp"
#include "duckdb/planner/parsed_data/bound_create_table_info.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/parser/constraints/list.hpp"
#include "duckdb/common/unordered_set.hpp"
#include "duckdb/parser/parsed_data/alter_info.hpp"
#include "duckdb/parser/parsed_data/alter_table_info.hpp"
#include "duckdb/parser/parsed_expression_iterator.hpp"

namespace duckdb {

IRCSchemaEntry::IRCSchemaEntry(Catalog &catalog, CreateSchemaInfo &info, IRCNamespaceInformation &namespace_info)
    : SchemaCatalogEntry(catalog, info), namespace_info(namespace_info) {
}

IRCSchemaEntry::~IRCSchemaEntry() {
}

IRCTransaction &GetUCTransaction(CatalogTransaction transaction) {
	if (!transaction.transaction) {
		throw InternalException("No transaction!?");
	}
	return transaction.transaction->Cast<IRCTransaction>();
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateTable(CatalogTransaction transaction, BoundCreateTableInfo &info) {
	throw NotImplementedException("Create Table");
}

void IRCSchemaEntry::DropEntry(ClientContext &context, DropInfo &info) {
	throw NotImplementedException("Drop Entry");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateFunction(CatalogTransaction transaction, CreateFunctionInfo &info) {
	throw BinderException("Iceberg databases do not support creating functions");
}

void ICUnqualifyColumnRef(ParsedExpression &expr) {
	if (expr.type == ExpressionType::COLUMN_REF) {
		auto &colref = expr.Cast<ColumnRefExpression>();
		auto name = std::move(colref.column_names.back());
		colref.column_names = {std::move(name)};
		return;
	}
	ParsedExpressionIterator::EnumerateChildren(expr, ICUnqualifyColumnRef);
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateIndex(CatalogTransaction transaction, CreateIndexInfo &info,
                                                       TableCatalogEntry &table) {
	throw NotImplementedException("Create Index");
}

string GetUCCreateView(CreateViewInfo &info) {
	throw NotImplementedException("Get Create View");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateView(CatalogTransaction transaction, CreateViewInfo &info) {
	throw NotImplementedException("Create View");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateType(CatalogTransaction transaction, CreateTypeInfo &info) {
	throw BinderException("Iceberg databases do not support creating types");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateSequence(CatalogTransaction transaction, CreateSequenceInfo &info) {
	throw BinderException("Iceberg databases do not support creating sequences");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateTableFunction(CatalogTransaction transaction,
                                                               CreateTableFunctionInfo &info) {
	throw BinderException("Iceberg databases do not support creating table functions");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateCopyFunction(CatalogTransaction transaction,
                                                              CreateCopyFunctionInfo &info) {
	throw BinderException("Iceberg databases do not support creating copy functions");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreatePragmaFunction(CatalogTransaction transaction,
                                                                CreatePragmaFunctionInfo &info) {
	throw BinderException("Iceberg databases do not support creating pragma functions");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::CreateCollation(CatalogTransaction transaction, CreateCollationInfo &info) {
	throw BinderException("Iceberg databases do not support creating collations");
}

void IRCSchemaEntry::Alter(CatalogTransaction transaction, AlterInfo &info) {
	throw NotImplementedException("Alter Schema Entry");
}

void IRCSchemaEntry::Scan(ClientContext &context, CatalogType type,
                          const std::function<void(CatalogEntry &)> &callback) {
	if (!CatalogTypeIsSupported(type)) {
		return;
	}
	GetCatalogSet(type).Scan(context, callback);
}
void IRCSchemaEntry::Scan(CatalogType type, const std::function<void(CatalogEntry &)> &callback) {
	throw NotImplementedException("Scan without context not supported");
}

optional_ptr<CatalogEntry> IRCSchemaEntry::LookupEntry(CatalogTransaction transaction,
                                                       const EntryLookupInfo &lookup_info) {
	auto type = lookup_info.GetCatalogType();
	if (type != CatalogType::TABLE_ENTRY) {
		throw NotImplementedException("The Iceberg REST Catalog only contains tables");
	}

	auto &entry_name = lookup_info.GetEntryName();
	auto &context = transaction.GetContext();
	auto &irc_transaction = transaction.transaction->Cast<IRCTransaction>();
	auto entry = irc_transaction.GetTable(context, entry_name);
	if (!entry) {
		if (if_not_found != OnEntryNotFound::RETURN_NULL) {
			throw CatalogException(lookup_info.GetErrorContext(), "Table with name \"%s\" not found", schema_name);
		}
		return nullptr;
	}
	return entry->Cast<CatalogEntry>();
}

// IRCCatalogSet &IRCSchemaEntry::GetCatalogSet(CatalogType type) {
//	switch (type) {
//	case CatalogType::TABLE_ENTRY:
//	case CatalogType::VIEW_ENTRY:
//		return tables;
//	default:
//		throw InternalException("Type not supported for GetCatalogSet");
//	}
//}

} // namespace duckdb
