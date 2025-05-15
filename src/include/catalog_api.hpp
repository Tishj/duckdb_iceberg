
#pragma once

#include "duckdb/common/types.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "duckdb/parser/parsed_data/create_secret_info.hpp"
#include "iceberg_metadata.hpp"
#include "storage/irc_schema_entry.hpp"
#include "rest_catalog/objects/load_table_result.hpp"

namespace duckdb {

class IRCatalog;

struct IRCAPITableCredentials {
	unique_ptr<CreateSecretInput> config;
	vector<CreateSecretInput> storage_credentials;
};

class IRCAPI {
public:
	static const string API_VERSION_1;

	static IRCAPITableCredentials GetTableCredentials(ClientContext &context, IRCatalog &catalog, const string &schema,
	                                                  const string &table, const string &secret_base_name);
	static vector<string> GetCatalogs(ClientContext &context, IRCatalog &catalog);
	// static vector<IRCAPITable> GetTables(ClientContext &context, IRCatalog &catalog, const string &schema);
	static rest_api_objects::LoadTableResult &&GetTable(ClientContext &context, IRCatalog &catalog,
	                                                    const string &schema, const string &table_name);
};

} // namespace duckdb
