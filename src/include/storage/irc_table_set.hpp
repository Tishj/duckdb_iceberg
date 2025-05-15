
#pragma once

#include "storage/irc_catalog_set.hpp"
#include "storage/irc_table_entry.hpp"

namespace duckdb {
struct CreateTableInfo;
class ICResult;
class IRCSchemaEntry;

class ICInSchemaSet : public IRCCatalogSet {
public:
	ICInSchemaSet(IRCSchemaEntry &schema);

	optional_ptr<CatalogEntry> CreateEntry(unique_ptr<CatalogEntry> entry) override;

protected:
	IRCSchemaEntry &schema;
};

// class ICTableSet : public ICInSchemaSet {
// public:
//	explicit ICTableSet(IRCSchemaEntry &schema);

// public:
//	optional_ptr<CatalogEntry> CreateTable(ClientContext &context, BoundCreateTableInfo &info);

// protected:
//	void LoadEntries(ClientContext &context) override;
//	void FillEntry(ClientContext &context, unique_ptr<CatalogEntry> &entry) override;
//};

} // namespace duckdb
