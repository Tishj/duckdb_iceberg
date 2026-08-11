#include "iceberg_attach.hpp"
#include "catalog/rest/iceberg_catalog.hpp"

#include "catalog/rest/storage/iceberg_authorization.hpp"
#include "catalog/rest/storage/authorization/oauth2.hpp"
#include "catalog/rest/storage/authorization/sigv4.hpp"
#include "catalog/rest/storage/authorization/none.hpp"
namespace duckdb {

unique_ptr<Catalog> IcebergAttach::Attach(optional_ptr<StorageExtensionInfo> storage_info, ClientContext &context,
                                          AttachedDatabase &db, const string &name, AttachInfo &info,
                                          AttachOptions &options) {
	IcebergAttachOptions attach_options;
	attach_options.warehouse = info.path;
	attach_options.name = name;

	// check if we have a secret provided
	Identifier default_schema;
	string authorization_type_string;
	string access_mode_string;
	string storage_credential_source_string;
	//! First handle generic attach options
	for (auto &entry : info.options) {
		auto lower_name = StringUtil::Lower(entry.first);
		if (lower_name == "type" || lower_name == "read_only") {
			continue;
		}

		if (lower_name == "authorization_type") {
			authorization_type_string = StringUtil::Lower(entry.second.ToString());
		} else if (lower_name == "access_delegation_mode") {
			access_mode_string = StringUtil::Lower(entry.second.ToString());
		} else if (lower_name == "uri") {
			attach_options.uri = entry.second.ToString();
			StringUtil::RTrim(attach_options.uri, "/");
		} else if (lower_name == "warehouse") {
			attach_options.warehouse = entry.second.ToString();
		} else if (lower_name == "supported_endpoints") {
			if (entry.second.type().id() != LogicalTypeId::LIST) {
				throw InvalidInputException("'SUPPORTED_ENDPOINTS' must be a list of strings");
			}
			for (auto &endpoint : ListValue::GetChildren(entry.second)) {
				attach_options.supported_endpoints.push_back(
				    endpoint.DefaultCastAs(LogicalType::VARCHAR).GetValue<string>());
			}
		} else if (lower_name == "storage_region") {
			attach_options.storage_region = entry.second.ToString();
		} else if (lower_name == "storage_endpoint") {
			attach_options.storage_endpoint = entry.second.ToString();
		} else if (lower_name == "storage_credential_source") {
			storage_credential_source_string = StringUtil::Lower(entry.second.ToString());
		} else if (lower_name == "stage_create_tables") {
			auto result = entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
			attach_options.stage_create_tables = result;
		} else if (lower_name == "disable_multi_table_commit") {
			attach_options.disable_multi_table_commit =
			    entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
		} else if (lower_name == "skip_create_table_metadata_updates") {
			attach_options.skip_create_table_metadata_updates =
			    entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
		} else if (lower_name == "remove_files_on_delete") {
			attach_options.remove_files_on_delete = entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
		} else if (lower_name == "support_nested_namespaces") {
			attach_options.support_nested_namespaces =
			    entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
		} else if (lower_name == "purge_requested") {
			attach_options.purge_requested = entry.second.DefaultCastAs(LogicalType::BOOLEAN).GetValue<bool>();
		} else if (lower_name == "default_schema") {
			default_schema = Identifier(entry.second.ToString());
		} else if (lower_name == "encode_entire_prefix") {
			attach_options.encode_entire_prefix = true;
		} else if (lower_name == "max_table_staleness") {
			auto interval_option = entry.second.DefaultCastAs(LogicalType::INTERVAL);
			auto interval_value = interval_option.GetValue<interval_t>();
			int64_t interval_in_micros = 0;
			if (!Interval::TryGetMicro(interval_value, interval_in_micros)) {
				throw ConversionException("Could not get interval information from %s", interval_option.ToString());
			}
			attach_options.max_table_staleness_micros = interval_in_micros;
		} else {
			attach_options.options.emplace(std::move(entry));
		}
	}
	//! Then check the authorization type
	if (!authorization_type_string.empty()) {
		attach_options.authorization_type = IcebergAuthorization::TypeFromString(authorization_type_string);
	}
	if (!storage_credential_source_string.empty()) {
		if (storage_credential_source_string == "vended") {
			attach_options.storage_credential_source = IRCStorageCredentialSource::VENDED;
		} else if (storage_credential_source_string == "catalog") {
			attach_options.storage_credential_source = IRCStorageCredentialSource::CATALOG;
		} else {
			throw InvalidInputException(
			    "Unrecognized storage credential source '%s'. Supported options are 'vended' and 'catalog'",
			    storage_credential_source_string);
		}
	}
	if (!access_mode_string.empty()) {
		if (access_mode_string == "vended_credentials") {
			attach_options.access_mode = IRCAccessDelegationMode::VENDED_CREDENTIALS;
		} else if (access_mode_string == "none") {
			attach_options.access_mode = IRCAccessDelegationMode::NONE;
		} else {
			throw InvalidInputException(
			    "Unrecognized access mode '%s'. Supported options are 'vended_credentials' and 'none'",
			    access_mode_string);
		}
	}
	if (attach_options.authorization_type == IcebergAuthorizationType::INVALID) {
		attach_options.authorization_type = IcebergAuthorizationType::OAUTH2;
	}

	//! Finally, create the auth_handler class from the authorization_type and the remaining options
	unique_ptr<IcebergAuthorization> auth_handler;
	switch (attach_options.authorization_type) {
	case IcebergAuthorizationType::OAUTH2: {
		auth_handler = OAuth2Authorization::FromAttachOptions(db, context, attach_options);
		break;
	}
	case IcebergAuthorizationType::SIGV4: {
		auth_handler = SIGV4Authorization::FromAttachOptions(db, attach_options);
		break;
	}
	case IcebergAuthorizationType::NONE: {
		auth_handler = NoneAuthorization::FromAttachOptions(db, attach_options);
		break;
	}
	default:
		throw InternalException("Authorization Type (%s) not implemented", authorization_type_string);
	}
	if (attach_options.storage_credential_source == IRCStorageCredentialSource::CATALOG &&
	    attach_options.authorization_type != IcebergAuthorizationType::SIGV4) {
		throw InvalidConfigurationException("STORAGE_CREDENTIAL_SOURCE 'catalog' requires SigV4 authorization");
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

	if (attach_options.uri.empty()) {
		throw InvalidConfigurationException("Missing 'uri' option for Iceberg attach");
	}

	D_ASSERT(auth_handler);
	auto catalog =
	    make_uniq<IcebergCatalog>(db, options.access_mode, std::move(auth_handler), attach_options, default_schema);
	//! Remember the raw attach options so that a later ATTACH OR REPLACE can detect when they change.
	catalog->SetAttachOptions(options.options);
	catalog->GetConfig(context);
	if (!default_schema.empty() &&
	    !IRCAPI::VerifySchemaExistence(context, *catalog, default_schema.GetIdentifierName())) {
		throw InvalidConfigurationException("default_schema '%s' does not exist", default_schema);
	}
	return std::move(catalog);
}

} // namespace duckdb
