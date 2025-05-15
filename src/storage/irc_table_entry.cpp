#include "storage/irc_catalog.hpp"
#include "storage/irc_schema_entry.hpp"
#include "storage/irc_table_entry.hpp"
#include "duckdb/storage/statistics/base_statistics.hpp"
#include "duckdb/storage/table_storage_info.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/main/secret/secret_manager.hpp"
#include "duckdb/catalog/catalog_entry/table_function_catalog_entry.hpp"
#include "duckdb/parser/tableref/table_function_ref.hpp"
#include "catalog_api.hpp"
#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "storage/authorization/sigv4.hpp"
#include "duckdb/common/multi_file/multi_file_reader.hpp"
#include "duckdb/planner/tableref/bound_at_clause.hpp"

#include "rest_catalog/objects/list.hpp"

namespace duckdb {

ICTableEntry::ICTableEntry(Catalog &catalog, IRCSchemaEntry &irc_schema, CreateTableInfo &info,
                           IcebergTableInformation &table_info)
    : TableCatalogEntry(catalog, irc_schema, info), irc_schema(irc_schema), table_info(table_info) {
	this->internal = false;
}

unique_ptr<BaseStatistics> ICTableEntry::GetStatistics(ClientContext &context, column_t column_id) {
	return nullptr;
}

void ICTableEntry::BindUpdateConstraints(Binder &binder, LogicalGet &, LogicalProjection &, LogicalUpdate &,
                                         ClientContext &) {
	throw NotImplementedException("BindUpdateConstraints");
}

static void AddTimeTravelInformation(named_parameter_map_t &param_map, BoundAtClause &at) {
	auto &unit = at.Unit();
	auto &value = at.GetValue();

	if (StringUtil::CIEquals(unit, "version")) {
		param_map.emplace("snapshot_from_id", value);
	} else if (StringUtil::CIEquals(unit, "timestamp")) {
		param_map.emplace("snapshot_from_timestamp", value);
	} else {
		throw InvalidInputException(
		    "Unit '%s' for time travel is not valid, supported options are 'version' and 'timestamp'", unit);
	}
}

string ICTableEntry::PrepareIcebergScanFromEntry(ClientContext &context) {
	auto &metadata = table_info.metadata.metadata;
	auto &ic_catalog = catalog.Cast<IRCatalog>();
	auto &secret_manager = SecretManager::Get(context);

	D_ASSERT(metadata.has_location);
	auto schema_name = irc_schema.name;
	auto table_name =
	    // Get Credentials from IRC API
	    auto secret_base_name =
	        StringUtil::Format("__internal_ic_%s__%s__%s", metadata.table_uuid, schema_name, table_name);
	auto table_credentials =
	    IRCAPI::GetTableCredentials(context, ic_catalog, schema_name, table_name, secret_base_name);
	// First check if table credentials are set (possible the IC catalog does not return credentials)

	if (table_credentials.config) {
		auto &info = *table_credentials.config;
		D_ASSERT(info.scope.empty());
		//! Limit the scope to the metadata location
		string lc_storage_location;
		lc_storage_location.resize(metadata.location.size());
		std::transform(metadata.location.begin(), metadata.location.end(), lc_storage_location.begin(), ::tolower);
		size_t metadata_pos = lc_storage_location.find("metadata");
		if (metadata_pos != string::npos) {
			info.scope = {metadata.location.substr(0, metadata_pos)};
		} else {
			throw InvalidInputException("Substring not found");
		}

		if (StringUtil::StartsWith(ic_catalog.uri, "glue")) {
			auto &sigv4_auth = ic_catalog.auth_handler->Cast<SIGV4Authorization>();
			//! Override the endpoint if 'glue' is the host of the catalog
			auto secret_entry = IRCatalog::GetStorageSecret(context, sigv4_auth.secret);
			auto kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
			auto region = kv_secret.TryGetValue("region").ToString();
			auto endpoint = "s3." + region + ".amazonaws.com";
			info.options["endpoint"] = endpoint;
		} else if (StringUtil::StartsWith(ic_catalog.uri, "s3tables")) {
			auto &sigv4_auth = ic_catalog.auth_handler->Cast<SIGV4Authorization>();
			//! Override all the options if 's3tables' is the host of the catalog
			auto secret_entry = IRCatalog::GetStorageSecret(context, sigv4_auth.secret);
			auto kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
			auto substrings = StringUtil::Split(ic_catalog.warehouse, ":");
			D_ASSERT(substrings.size() == 6);
			auto region = substrings[3];
			auto endpoint = "s3." + region + ".amazonaws.com";
			info.options = {{"key_id", kv_secret.TryGetValue("key_id").ToString()},
			                {"secret", kv_secret.TryGetValue("secret").ToString()},
			                {"session_token", kv_secret.TryGetValue("session_token").IsNull()
			                                      ? ""
			                                      : kv_secret.TryGetValue("session_token").ToString()},
			                {"region", region},
			                {"endpoint", endpoint}};
		}

		(void)secret_manager.CreateSecret(context, info);
	}

	for (auto &info : table_credentials.storage_credentials) {
		(void)secret_manager.CreateSecret(context, info);
	}
	return metadata.location;
}

TableFunction ICTableEntry::GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data,
                                            const EntryLookupInfo &lookup) {
	auto &db = DatabaseInstance::GetDatabase(context);
	auto &iceberg_scan_function_set = ExtensionUtil::GetTableFunction(db, "iceberg_scan");
	auto iceberg_scan_function =
	    iceberg_scan_function_set.functions.GetFunctionByArguments(context, {LogicalType::VARCHAR});
	auto storage_location = PrepareIcebergScanFromEntry(context);

	named_parameter_map_t param_map;
	vector<LogicalType> return_types;
	vector<string> names;
	TableFunctionRef empty_ref;

	// Set the S3 path as input to table function
	auto at = lookup.GetAtClause();
	if (at) {
		AddTimeTravelInformation(param_map, *at);
	}
	vector<Value> inputs = {storage_location};
	TableFunctionBindInput bind_input(inputs, param_map, return_types, names, nullptr, nullptr, iceberg_scan_function,
	                                  empty_ref);

	auto result = iceberg_scan_function.bind(context, bind_input, return_types, names);
	bind_data = std::move(result);

	return iceberg_scan_function;
}

TableFunction ICTableEntry::GetScanFunction(ClientContext &context, unique_ptr<FunctionData> &bind_data) {
	throw InternalException("ICTableEntry::GetScanFunction called without entry lookup info");
}

virtual_column_map_t ICTableEntry::GetVirtualColumns() const {
	virtual_column_map_t result;
	result.insert(make_pair(MultiFileReader::COLUMN_IDENTIFIER_FILE_ROW_NUMBER,
	                        TableColumn("file_row_number", LogicalType::BIGINT)));
	result.insert(
	    make_pair(MultiFileReader::COLUMN_IDENTIFIER_FILE_INDEX, TableColumn("file_index", LogicalType::UBIGINT)));
	result.insert(make_pair(COLUMN_IDENTIFIER_EMPTY, TableColumn("", LogicalType::BOOLEAN)));
	return result;
}

vector<column_t> ICTableEntry::GetRowIdColumns() const {
	vector<column_t> result;
	result.emplace_back(MultiFileReader::COLUMN_IDENTIFIER_FILE_INDEX);
	result.emplace_back(MultiFileReader::COLUMN_IDENTIFIER_FILE_ROW_NUMBER);
	return result;
}

TableStorageInfo ICTableEntry::GetStorageInfo(ClientContext &context) {
	TableStorageInfo result;
	// TODO fill info
	return result;
}

} // namespace duckdb
