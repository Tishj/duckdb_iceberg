#include "catalog/rest/iceberg_rest_catalog_backend.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/api/catalog_api.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table.hpp"
#include "duckdb/logging/logger.hpp"
#include "duckdb/main/database.hpp"
#include "iceberg_logging.hpp"

namespace duckdb {

IcebergRESTCatalogBackend::IcebergRESTCatalogBackend(unique_ptr<IcebergAuthorization> auth_handler,
                                                     const IcebergAttachOptions &options)
    : auth_handler(std::move(auth_handler)), base_uri(options.catalog_uri), attach_options(options),
      warehouse(options.warehouse), table_request_cache(attach_options) {
}

IcebergRESTCatalogBackend::~IcebergRESTCatalogBackend() = default;

IcebergRESTCatalogBackend &IcebergRESTCatalogBackend::Get(IcebergCatalog &catalog) {
	auto backend = dynamic_cast<IcebergRESTCatalogBackend *>(&catalog.GetBackend());
	if (!backend) {
		throw NotImplementedException("This operation requires an Iceberg REST catalog backend");
	}
	return *backend;
}

string IcebergRESTCatalogBackend::GetDBPath() const {
	return warehouse;
}

void LoadTableResultCache::EvictIfCurrent(const IcebergTable &table) {
	annotated_lock_guard<annotated_mutex> guard(lock);
	auto it = tables.find(table.GetTableKey());
	if (it == tables.end()) {
		return;
	}
	auto data = dynamic_cast<const IcebergRESTTableData *>(table.backend_data.get());
	if (!data || it->second.load_table_result.get() != data->initialization_source.get()) {
		return;
	}
	tables.erase(it);
}

IRCEndpointBuilder IcebergRESTCatalogBackend::GetBaseUrl() const {
	auto url_builder = IRCEndpointBuilder();
	url_builder.SetHost(base_uri);
	url_builder.AddPathComponent(IRCPathComponent::RegularComponent(version));

	return url_builder;
}

unique_ptr<SecretEntry> IcebergRESTCatalogBackend::GetStorageSecret(ClientContext &context, const string &secret_name) {
	auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

	case_insensitive_set_t accepted_secret_types {"s3", "aws"};

	if (!secret_name.empty()) {
		auto secret_entry = context.db->GetSecretManager().GetSecretByName(transaction, secret_name);
		if (secret_entry) {
			auto secret_type = secret_entry->secret->GetType();
			if (accepted_secret_types.count(secret_type.GetIdentifierName())) {
				return secret_entry;
			}
			throw InvalidConfigurationException(
			    "Found a secret by the name of '%s', but it is not of an accepted type for a 'secret', "
			    "accepted types are: 's3' or 'aws', found '%s'",
			    secret_name, secret_type);
		}
		throw InvalidConfigurationException(
		    "No secret by the name of '%s' could be found, consider changing the 'secret'", secret_name);
	}

	for (auto &type : accepted_secret_types) {
		if (secret_name.empty()) {
			//! Lookup the default secret for this type
			auto secret_entry =
			    context.db->GetSecretManager().GetSecretByName(transaction, StringUtil::Format("__default_%s", type));
			if (secret_entry) {
				return secret_entry;
			}
		}
		auto secret_match = context.db->GetSecretManager().LookupSecret(transaction, type + "://", type);
		if (secret_match.HasMatch()) {
			return std::move(secret_match.secret_entry);
		}
	}
	throw InvalidConfigurationException("Could not find a valid storage secret (s3 or aws)");
}

unique_ptr<SecretEntry> IcebergRESTCatalogBackend::GetIcebergSecret(ClientContext &context, const string &secret_name) {
	auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);
	unique_ptr<SecretEntry> secret_entry = nullptr;
	if (secret_name.empty()) {
		//! Try to find any secret with type 'iceberg'
		auto secret_match = context.db->GetSecretManager().LookupSecret(transaction, "", "iceberg");
		if (!secret_match.HasMatch()) {
			return nullptr;
		}
		secret_entry = std::move(secret_match.secret_entry);
	} else {
		secret_entry = context.db->GetSecretManager().GetSecretByName(transaction, secret_name);
	}
	return secret_entry;
}

unique_ptr<SecretEntry> IcebergRESTCatalogBackend::GetHTTPSecret(ClientContext &context, const string &secret_name) {
	auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);
	unique_ptr<SecretEntry> secret_entry = nullptr;

	if (!secret_name.empty()) {
		secret_entry = context.db->GetSecretManager().GetSecretByName(transaction, secret_name);
		if (!secret_entry) {
			throw InternalException("Secret '%s' not found", secret_name);
		}
		auto http_kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
		bool has_proxy = !http_kv_secret.TryGetValue("http_proxy").IsNull();
		if (has_proxy) {
			return secret_entry;
		}
	}
	auto secret_match = context.db->GetSecretManager().LookupSecret(transaction, "", "http");
	if (!secret_match.HasMatch()) {
		return nullptr;
	}
	secret_entry = std::move(secret_match.secret_entry);
	return secret_entry;
}
void IcebergRESTCatalogBackend::AddDefaultSupportedEndpoints() {
	// insert namespaces based on REST API spec.
	// List namespaces
	supported_urls.insert("GET /v1/{prefix}/namespaces");
	// create namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces");
	// Load metadata for a Namespace
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}");
	// Drop a namespace
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}");
	// set or remove properties on a namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/properties");
	// list all table identifiers
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables");
	// create table in the namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables");
	// get table from the catalog
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// commit updates to a tbale
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// drop table from a catalog
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// Register a table using given metadata file location.
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/register");
	// send metrics report to this endpoint to be processed by the backend
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables/{table}/metrics");
	// Rename a table from one identifier to another.
	supported_urls.insert("POST /v1/{prefix}/tables/rename");
	// commit updates to multiple tables in an atomic transaction
	supported_urls.insert("POST /v1/{prefix}/transactions/commit");
}

void IcebergRESTCatalogBackend::AddS3TablesEndpoints() {
	// insert namespaces based on REST API spec.
	// List namespaces
	supported_urls.insert("GET /v1/{prefix}/namespaces");
	// create namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces");
	// Load metadata for a Namespace
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}");
	// Drop a namespace
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}");
	// list all table identifiers
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables");
	// create table in the namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables");
	// get table from the catalog
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// commit updates to a table
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// drop table from a catalog
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// Rename a table from one identifier to another.
	supported_urls.insert("POST /v1/{prefix}/tables/rename");
	// table exists
	supported_urls.insert("HEAD /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// namespace exists
	supported_urls.insert("HEAD /v1/{prefix}/namespaces/{namespace}");
}

void IcebergRESTCatalogBackend::AddGlueEndpoints() {
	// insert namespaces based on REST API spec.
	// List namespaces
	supported_urls.insert("GET /v1/{prefix}/namespaces");
	// create namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces");
	// Load metadata for a Namespace
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}");
	// Drop a namespace
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}");
	// list all table identifiers
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables");
	// create table in the namespace
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables");
	// get table from the catalog
	supported_urls.insert("GET /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// table exists
	supported_urls.insert("HEAD /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// commit updates to a table
	supported_urls.insert("POST /v1/{prefix}/namespaces/{namespace}/tables/{table}");
	// drop table from a catalog
	supported_urls.insert("DELETE /v1/{prefix}/namespaces/{namespace}/tables/{table}");
}

void IcebergRESTCatalogBackend::ParsePrefix() {
	// save overrides and defaults.
	// See https://iceberg.apache.org/docs/latest/configuration/#catalog-properties for sometimes used catalog
	// properties
	auto default_prefix_it = defaults.find("prefix");
	auto override_prefix_it = overrides.find("prefix");

	const string *prefix_property = nullptr;
	if (default_prefix_it != defaults.end()) {
		prefix_property = &default_prefix_it->second;
	}
	// Sometimes the prefix is in the overrides. Prefer the override prefix.
	if (override_prefix_it != overrides.end()) {
		prefix_property = &override_prefix_it->second;
	}
	if (!prefix_property) {
		return;
	}

	auto decoded_prefix = StringUtil::URLDecode(*prefix_property);
	if (attach_options.encode_entire_prefix || !StringUtil::Equals(decoded_prefix, *prefix_property)) {
		prefix.push_back(std::move(decoded_prefix));
	} else {
		prefix = StringUtil::Split(decoded_prefix, '/');
	}
}

void IcebergRESTCatalogBackend::ParseNamespaceSeparator() {
	auto default_namespace_separator_it = defaults.find("namespace-separator");
	auto override_namespace_separator_it = overrides.find("namespace-separator");

	const string *namespace_separator_property = nullptr;
	if (default_namespace_separator_it != defaults.end()) {
		namespace_separator_property = &default_namespace_separator_it->second;
	}
	// Sometimes the namespace_separator is in the overrides. Prefer the override namespace_separator.
	if (override_namespace_separator_it != overrides.end()) {
		namespace_separator_property = &override_namespace_separator_it->second;
	}
	if (!namespace_separator_property) {
		return;
	}
	namespace_separator = *namespace_separator_property;
}

void IcebergRESTCatalogBackend::GetConfig(ClientContext &context, IcebergEndpointType &endpoint_type) {
	// set the prefix to be empty. To get the config endpoint,
	// we cannot add a default prefix.
	D_ASSERT(prefix.empty());

	// For AWS Glue, ":" means "default account catalog" — omit the warehouse param
	string effective_warehouse = warehouse;
	if (endpoint_type == IcebergEndpointType::AWS_GLUE && warehouse == ":") {
		effective_warehouse = "";
	}
	auto catalog_config = IRCAPI::GetCatalogConfig(context, *this, effective_warehouse);
	overrides = catalog_config.overrides;
	defaults = catalog_config.defaults;
	auto uri_override_it = overrides.find("uri");
	if (uri_override_it != overrides.end()) {
		base_uri = uri_override_it->second;
		StringUtil::RTrim(base_uri, "/");
	}
	ParsePrefix();
	ParseNamespaceSeparator();

	if (auto &endpoints = catalog_config.endpoints) {
		for (auto &endpoint : *endpoints) {
			supported_urls.insert(endpoint);
		}
	}
	// should be if s3tables
	if (!catalog_config.endpoints && endpoint_type == IcebergEndpointType::AWS_S3TABLES) {
		supported_urls.clear();
		AddS3TablesEndpoints();
	} else if (!catalog_config.endpoints && endpoint_type == IcebergEndpointType::AWS_GLUE) {
		supported_urls.clear();
		AddGlueEndpoints();
	} else if (!catalog_config.endpoints) {
		AddDefaultSupportedEndpoints();
	}

	if (prefix.empty()) {
		DUCKDB_LOG(context, IcebergLogType, "No prefix found for catalog with warehouse value %s", warehouse);
	}
}

} // namespace duckdb
