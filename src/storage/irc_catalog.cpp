#include "storage/irc_schema_entry.hpp"
#include "storage/irc_transaction.hpp"
#include "catalog_api.hpp"
#include "catalog_utils.hpp"
#include "iceberg_utils.hpp"
#include "api_utils.hpp"
#include "duckdb/storage/database_size.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"
#include "duckdb/main/attached_database.hpp"
#include "rest_catalog/objects/catalog_config.hpp"
#include "storage/irc_catalog.hpp"
#include "curl.hpp"

#include <regex>
#include "storage/irc_authorization.hpp"
#include "storage/authorization/oauth2.hpp"
#include "storage/authorization/sigv4.hpp"

using namespace duckdb_yyjson;

namespace duckdb {

IRCatalog::IRCatalog(AttachedDatabase &db_p, AccessMode access_mode, unique_ptr<IRCAuthorization> auth_handler,
                     const string &warehouse, const string &uri, const string &version)
    : Catalog(db_p), access_mode(access_mode), auth_handler(std::move(auth_handler)), warehouse(warehouse), uri(uri),
      version(version) {
	if (version.empty()) {
		throw InternalException("version can not be empty");
	}
}

IRCatalog::~IRCatalog() = default;

//===--------------------------------------------------------------------===//
// Catalog API
//===--------------------------------------------------------------------===//

void IRCatalog::Initialize(bool load_builtin) {
}

void IRCatalog::ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) {
	throw NotImplementedException("TODO: ScanSchemas not implemented");
}

optional_ptr<SchemaCatalogEntry> IRCatalog::LookupSchema(CatalogTransaction transaction,
                                                         const EntryLookupInfo &schema_lookup,
                                                         OnEntryNotFound if_not_found) {
	auto &schema_name = schema_lookup.GetEntryName();
	auto &context = transaction.GetContext();
	auto &irc_transaction = transaction.transaction->Cast<IRCTransaction>();
	auto entry = irc_transaction.GetSchema(context, schema_name);
	if (!entry) {
		if (if_not_found != OnEntryNotFound::RETURN_NULL) {
			throw CatalogException(schema_lookup.GetErrorContext(), "Schema with name \"%s\" not found", schema_name);
		}
		return nullptr;
	}
	return entry->Cast<SchemaCatalogEntry>();
}

optional_ptr<CatalogEntry> IRCatalog::CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) {
	throw NotImplementedException("IRCatalog::CreateSchema not implemented");
}

void IRCatalog::DropSchema(ClientContext &context, DropInfo &info) {
	throw NotImplementedException("IRCatalog::DropSchema not implemented");
}

PhysicalOperator &IRCatalog::PlanInsert(ClientContext &context, PhysicalPlanGenerator &planner, LogicalInsert &op,
                                        optional_ptr<PhysicalOperator> plan) {
	throw NotImplementedException("IRCatalog PlanInsert");
}
PhysicalOperator &IRCatalog::PlanCreateTableAs(ClientContext &context, PhysicalPlanGenerator &planner,
                                               LogicalCreateTable &op, PhysicalOperator &plan) {
	throw NotImplementedException("IRCatalog PlanCreateTableAs");
}
PhysicalOperator &IRCatalog::PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op,
                                        PhysicalOperator &plan) {
	throw NotImplementedException("IRCatalog PlanDelete");
}
PhysicalOperator &IRCatalog::PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner, LogicalUpdate &op,
                                        PhysicalOperator &plan) {
	throw NotImplementedException("IRCatalog PlanUpdate");
}
unique_ptr<LogicalOperator> IRCatalog::BindCreateIndex(Binder &binder, CreateStatement &stmt, TableCatalogEntry &table,
                                                       unique_ptr<LogicalOperator> plan) {
	throw NotImplementedException("IRCatalog BindCreateIndex");
}

bool IRCatalog::InMemory() {
	return false;
}

string IRCatalog::GetDBPath() {
	return warehouse;
}

DatabaseSize IRCatalog::GetDatabaseSize(ClientContext &context) {
	DatabaseSize size;
	return size;
}

//===--------------------------------------------------------------------===//
// Iceberg REST Catalog
//===--------------------------------------------------------------------===//

IRCEndpointBuilder IRCatalog::GetBaseUrl() const {
	auto base_url = IRCEndpointBuilder();
	base_url.SetHost(uri);
	base_url.AddPathComponent(version);
	return base_url;
}

unique_ptr<SecretEntry> IRCatalog::GetStorageSecret(ClientContext &context, const string &secret_name) {
	auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

	case_insensitive_set_t accepted_secret_types {"s3", "aws"};

	if (!secret_name.empty()) {
		auto secret_entry = context.db->GetSecretManager().GetSecretByName(transaction, secret_name);
		if (secret_entry) {
			auto secret_type = secret_entry->secret->GetType();
			if (accepted_secret_types.count(secret_type)) {
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

unique_ptr<SecretEntry> IRCatalog::GetIcebergSecret(ClientContext &context, const string &secret_name) {
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

void IRCatalog::GetConfig(ClientContext &context) {
	auto url = GetBaseUrl();
	// set the prefix to be empty. To get the config endpoint,
	// we cannot add a default prefix.
	D_ASSERT(prefix.empty());
	url.AddPathComponent("config");
	url.SetParam("warehouse", warehouse);
	RequestInput request_input;
	auto response = auth_handler->GetRequest(context, url, request_input);
	std::unique_ptr<yyjson_doc, YyjsonDocDeleter> doc(ICUtils::api_result_to_doc(response));
	auto *root = yyjson_doc_get_root(doc.get());
	auto catalog_config = rest_api_objects::CatalogConfig::FromJSON(root);

	overrides = catalog_config.overrides;
	defaults = catalog_config.defaults;
	// save overrides and defaults.
	// See https://iceberg.apache.org/docs/latest/configuration/#catalog-properties for sometimes used catalog
	// properties

	auto default_prefix_it = defaults.find("prefix");
	auto override_prefix_it = overrides.find("prefix");

	if (default_prefix_it != defaults.end()) {
		// sometimes there is a prefix in the defaults
		prefix = StringUtil::URLDecode(default_prefix_it->second);
		defaults.erase(default_prefix_it);
	}
	if (override_prefix_it != overrides.end()) {
		// sometimes the prefix in the overrides. Prefer the override prefix
		prefix = StringUtil::URLDecode(override_prefix_it->second);
		overrides.erase(override_prefix_it);
	}

	if (prefix.empty()) {
		DUCKDB_LOG_DEBUG(context, "iceberg.Catalog.HttpRequest", "No prefix found for catalog with warehouse value %s",
		                 warehouse);
	}
	// TODO: store optional endpoints param as well. We can enforce per catalog the endpoints that
	//  are allowed to be hit
}

// string IRCatalog::OptionalGetCachedValue(const string &url) {
//	std::lock_guard<std::mutex> lock(metadata_cache_mutex);
//	auto value = metadata_cache.find(url);
//	if (value != metadata_cache.end()) {
//		auto now = system_clock::now();
//		if (now < value->second->expires_at) {
//			return value->second->data;
//		}
//	}
//	return "";
//}

// bool IRCatalog::SetCachedValue(const string &url, const string &value,
//                               const rest_api_objects::LoadTableResult &result) {
//	//! FIXME: shouldn't this also store the 'storage-credentials' ??
//	if (!result.has_config) {
//		return false;
//	}
//	auto &credentials = result.config;
//	auto expires_at_it = credentials.find("s3.session-token-expires-at-ms");
//	if (expires_at_it == credentials.end()) {
//		return false;
//	}

//	auto &expires_at = expires_at_it->second;
//	auto epochMillis = std::stoll(expires_at);
//	auto expired_time = system_clock::time_point(milliseconds(epochMillis));
//	auto val = make_uniq<MetadataCacheValue>(value, expired_time);
//	{
//		std::lock_guard<std::mutex> lock(metadata_cache_mutex);
//		metadata_cache[url] = std::move(val);
//	}
//	return true;
//}

//===--------------------------------------------------------------------===//
// Attach
//===--------------------------------------------------------------------===//

// namespace
namespace {

enum class IcebergEndpointType : uint8_t { AWS_S3TABLES, AWS_GLUE, INVALID };

static IcebergEndpointType EndpointTypeFromString(const string &input) {
	D_ASSERT(StringUtil::Lower(input) == input);

	static const case_insensitive_map_t<IcebergEndpointType> mapping {{"glue", IcebergEndpointType::AWS_GLUE},
	                                                                  {"s3_tables", IcebergEndpointType::AWS_S3TABLES}};

	for (auto &entry : mapping) {
		if (entry.first == input) {
			return entry.second;
		}
	}
	set<string> options;
	for (auto &entry : mapping) {
		options.insert(entry.first);
	}
	throw InvalidConfigurationException("Unrecognized 'endpoint_type' (%s), accepted options are: %s", input,
	                                    StringUtil::Join(options, ", "));
}

} // namespace

//! Streamlined initialization for recognized catalog types

static void S3OrGlueAttachInternal(IcebergAttachOptions &input, const string &service, const string &region) {
	if (input.authorization_type != IRCAuthorizationType::INVALID) {
		throw InvalidConfigurationException("'endpoint_type' can not be combined with 'authorization_type'");
	}

	input.authorization_type = IRCAuthorizationType::SIGV4;
	input.endpoint = StringUtil::Format("%s.%s.amazonaws.com/iceberg", service, region);
}

static void S3TablesAttach(IcebergAttachOptions &input) {
	// extract region from the amazon ARN
	auto substrings = StringUtil::Split(input.warehouse, ":");
	if (substrings.size() != 6) {
		throw InvalidInputException("Could not parse S3 Tables ARN warehouse value");
	}
	auto region = substrings[3];
	S3OrGlueAttachInternal(input, "s3tables", region);
}

static bool SanityCheckGlueWarehouse(const string &warehouse) {
	// See: https://docs.aws.amazon.com/glue/latest/dg/connect-glu-iceberg-rest.html#prefix-catalog-path-parameters

	const std::regex patterns[] = {
	    std::regex("^:$"),                  // Default catalog ":" in current account
	    std::regex("^\\d{12}$"),            // Default catalog in a specific account
	    std::regex("^\\d{12}:[^:/]+$"),     // Specific catalog in a specific account
	    std::regex("^[^:]+/[^:]+$"),        // Nested catalog in the current account
	    std::regex("^\\d{12}:[^/]+/[^:]+$") // Nested catalog in a specific account
	};

	for (const auto &pattern : patterns) {
		if (std::regex_match(warehouse, pattern)) {
			return true;
		}
	}

	throw InvalidConfigurationException(
	    "Invalid Glue Catalog Format: '%s'. Expected format: ':', '12-digit account ID', "
	    "'catalog1/catalog2', or '12-digit accountId:catalog1/catalog2'.",
	    warehouse);
}

static void GlueAttach(ClientContext &context, IcebergAttachOptions &input) {
	SanityCheckGlueWarehouse(input.warehouse);

	string secret;
	auto secret_it = input.options.find("secret");
	if (secret_it != input.options.end()) {
		secret = secret_it->second.ToString();
	}

	// look up any s3 secret

	// if there is no secret, an error will be thrown
	auto secret_entry = IRCatalog::GetStorageSecret(context, secret);
	auto kv_secret = dynamic_cast<const KeyValueSecret &>(*secret_entry->secret);
	auto region = kv_secret.TryGetValue("region");

	if (region.IsNull()) {
		throw InvalidConfigurationException("Assumed catalog secret '%s' for catalog '%s' does not have a region",
		                                    secret_entry->secret->GetName(), input.name);
	}
	S3OrGlueAttachInternal(input, "glue", region.ToString());
}

unique_ptr<Catalog> IRCatalog::Attach(StorageExtensionInfo *storage_info, ClientContext &context, AttachedDatabase &db,
                                      const string &name, AttachInfo &info, AccessMode access_mode) {
	IRCEndpointBuilder endpoint_builder;

	string endpoint_type_string;
	string authorization_type_string;

	IcebergAttachOptions attach_options;
	attach_options.warehouse = info.path;
	attach_options.name = name;

	// check if we have a secret provided
	string secret_name;
	//! First handle generic attach options
	for (auto &entry : info.options) {
		auto lower_name = StringUtil::Lower(entry.first);
		if (lower_name == "type" || lower_name == "read_only") {
			continue;
		}

		if (lower_name == "endpoint_type") {
			endpoint_type_string = StringUtil::Lower(entry.second.ToString());
		} else if (lower_name == "authorization_type") {
			authorization_type_string = StringUtil::Lower(entry.second.ToString());
		} else if (lower_name == "endpoint") {
			attach_options.endpoint = StringUtil::Lower(entry.second.ToString());
			StringUtil::RTrim(attach_options.endpoint, "/");
		} else {
			attach_options.options.emplace(std::move(entry));
		}
	}

	//! Then check any if the 'endpoint_type' is set, for any well known catalogs
	if (!endpoint_type_string.empty()) {
		auto endpoint_type = EndpointTypeFromString(endpoint_type_string);
		switch (endpoint_type) {
		case IcebergEndpointType::AWS_GLUE: {
			GlueAttach(context, attach_options);
			break;
		}
		case IcebergEndpointType::AWS_S3TABLES: {
			S3TablesAttach(attach_options);
			break;
		}
		default:
			throw InternalException("Endpoint type (%s) not implemented", endpoint_type_string);
		}
	}

	//! Then check the authorization type
	if (!authorization_type_string.empty()) {
		if (attach_options.authorization_type != IRCAuthorizationType::INVALID) {
			throw InvalidConfigurationException("'authorization_type' can not be combined with 'endpoint_type'");
		}
		attach_options.authorization_type = IRCAuthorization::TypeFromString(authorization_type_string);
	}
	if (attach_options.authorization_type == IRCAuthorizationType::INVALID) {
		attach_options.authorization_type = IRCAuthorizationType::OAUTH2;
	}

	//! Finally, create the auth_handler class from the authorization_type and the remaining options
	unique_ptr<IRCAuthorization> auth_handler;
	switch (attach_options.authorization_type) {
	case IRCAuthorizationType::OAUTH2: {
		auth_handler = OAuth2Authorization::FromAttachOptions(context, attach_options);
		break;
	}
	case IRCAuthorizationType::SIGV4: {
		auth_handler = SIGV4Authorization::FromAttachOptions(attach_options);
		break;
	}
	default:
		throw InternalException("Authorization Type (%s) not implemented", authorization_type_string);
	}

	//! We throw if there are any additional options not handled by previous steps
	if (!attach_options.options.empty()) {
		set<string> unrecognized_options;
		for (auto &entry : attach_options.options) {
			unrecognized_options.insert(entry.first);
		}
		throw InvalidConfigurationException("Unhandled options found: %s",
		                                    StringUtil::Join(unrecognized_options, ", "));
	}

	if (attach_options.endpoint.empty()) {
		throw InvalidConfigurationException("Missing 'endpoint' option for Iceberg attach");
	}

	D_ASSERT(auth_handler);
	auto catalog = make_uniq<IRCatalog>(db, access_mode, std::move(auth_handler), attach_options.warehouse,
	                                    attach_options.endpoint);
	catalog->GetConfig(context);
	return std::move(catalog);
}

} // namespace duckdb
