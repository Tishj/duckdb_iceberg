
#pragma once

#include "catalog_api.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "storage/irc_transaction.hpp"

namespace duckdb {

class ICTableEntry : public TableCatalogEntry {
public:
	ICTableEntry(Catalog &catalog, IRCSchemaEntry &schema, CreateTableInfo &info, IcebergTableInformation &table_info);

	virtual_column_map_t GetVirtualColumns() const override;
	vector<column_t> GetRowIdColumns() const override;

public:
	unique_ptr<BaseStatistics> GetStatistics(ClientContext &context, column_t column_id) override;

	string PrepareIcebergScanFromEntry(ClientContext &context);
	TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) override;
	TableFunction GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data,
	                              const EntryLookupInfo &lookup) override;

	TableStorageInfo GetStorageInfo(ClientContext &context) override;

	void BindUpdateConstraints(Binder &binder, LogicalGet &get, LogicalProjection &proj, LogicalUpdate &update,
	                           ClientContext &context) override;

public:
	IRCSchemaEntry &irc_schema;
	IcebergTableInformation &table_info;
};

} // namespace duckdb
