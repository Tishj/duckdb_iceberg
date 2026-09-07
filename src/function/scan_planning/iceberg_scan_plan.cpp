#include "iceberg_functions.hpp"

namespace duckdb {

struct IcebergScanPlanBindData : public TableFunctionData {
	IcebergTableSchemaVersion &table_entry;
	IcebergCatalog &ic_catalog;
	IcebergSchemaEntry &ic_schema;

	IcebergScanPlanBindData(IcebergTableSchemaVersion &table_entry, IcebergCatalog &ic_catalog,
	                        IcebergSchemaEntry &ic_schema)
	    : table_entry(table_entry), ic_catalog(ic_catalog), ic_schema(ic_schema) {
	}
};

struct IcebergScanPlanGlobalState : public GlobalTableFunctionState {
	bool done = false;

	static unique_ptr<GlobalTableFunctionState> Init(ClientContext &context, TableFunctionInitInput &input) {
		return make_uniq<IcebergScanPlanGlobalState>();
	}
};

static unique_ptr<FunctionData> IcebergScanPlanBind(ClientContext &context, TableFunctionBindInput &input,
                                                    vector<LogicalType> &return_types, vector<Identifier> &names) {
	auto input_string = input.inputs[0].ToString();
	auto qualified_name = QualifiedName::ParseComponents(input_string);

	if (qualified_name.size() != 3) {
		throw InvalidInputException("Expected fully qualified table name (catalog.schema.table), got: %s",
		                            input_string);
	}

	EntryLookupInfo table_lookup(CatalogType::TABLE_ENTRY,
	                             QualifiedName(qualified_name[0], qualified_name[1], qualified_name[2]));
	auto catalog_entry = Catalog::GetEntry(context, table_lookup, OnEntryNotFound::THROW_EXCEPTION);

	if (catalog_entry->type != CatalogType::TABLE_ENTRY) {
		throw InvalidInputException("'%s' is not a table", input_string);
	}
	auto &table = catalog_entry->Cast<TableCatalogEntry>();
	if (table.catalog.GetCatalogType() != "iceberg") {
		throw InvalidInputException("Table '%s' is not an Iceberg REST catalog table", input_string);
	}

	auto &table_entry = catalog_entry->Cast<IcebergTableSchemaVersion>();
	auto &ic_catalog = table_entry.catalog.Cast<IcebergCatalog>();
	auto &ic_schema = table_entry.schema.Cast<IcebergSchemaEntry>();

	auto ret = make_uniq<IcebergScanPlanBindData>(table_entry, ic_catalog, ic_schema);

	names.emplace_back("file_path");
	types.emplace_back(LogicalType::VARCHAR);

	return std::move(ret);
}

static void IcebergScanPlanFunction(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
	auto &bind_data = data.bind_data->Cast<IcebergScanPlanBindData>();
	auto &global_state = data.global_state->Cast<IcebergScanPlanGlobalState>();

	if (global_state.done) {
		return;
	}
	global_state.done = true;
}

TableFunctionSet IcebergFunctions::GetIcebergScanPlanFunction() {
	TableFunctionSet function_set("iceberg_scan_plan");

	auto fun = TableFunction({LogicalType::VARCHAR}, IcebergScanPlanFunction, IcebergScanPlanBind,
	                         IcebergScanPlanGlobalState::Init);
	function_set.AddFunction(fun);

	return function_set;
}

} // namespace duckdb
