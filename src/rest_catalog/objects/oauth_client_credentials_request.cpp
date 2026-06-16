
#include "rest_catalog/objects/oauth_client_credentials_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

OAuthClientCredentialsRequest::OAuthClientCredentialsRequest(string grant_type_p, string client_id_p,
                                                             string client_secret_p, optional<string> scope_p)
    : grant_type(std::move(grant_type_p)), client_id(std::move(client_id_p)), client_secret(std::move(client_secret_p)),
      scope(std::move(scope_p)) {
}

OAuthClientCredentialsRequestBuilder::OAuthClientCredentialsRequestBuilder() {
}

OAuthClientCredentialsRequestBuilder &OAuthClientCredentialsRequestBuilder::SetGrantType(string value) {
	grant_type_.emplace(std::move(value));
	has_grant_type_ = true;
	return *this;
}

OAuthClientCredentialsRequestBuilder &OAuthClientCredentialsRequestBuilder::SetClientId(string value) {
	client_id_.emplace(std::move(value));
	has_client_id_ = true;
	return *this;
}

OAuthClientCredentialsRequestBuilder &OAuthClientCredentialsRequestBuilder::SetClientSecret(string value) {
	client_secret_.emplace(std::move(value));
	has_client_secret_ = true;
	return *this;
}

OAuthClientCredentialsRequestBuilder &OAuthClientCredentialsRequestBuilder::SetScope(string value) {
	scope_.emplace(std::move(value));
	return *this;
}

OAuthClientCredentialsRequest OAuthClientCredentialsRequestBuilder::Build() {
	if (!has_grant_type_) {
		throw InvalidInputException("OAuthClientCredentialsRequest required property 'grant_type' is missing");
	}
	if (!has_client_id_) {
		throw InvalidInputException("OAuthClientCredentialsRequest required property 'client_id' is missing");
	}
	if (!has_client_secret_) {
		throw InvalidInputException("OAuthClientCredentialsRequest required property 'client_secret' is missing");
	}
	auto result = OAuthClientCredentialsRequest(std::move(*grant_type_), std::move(*client_id_),
	                                            std::move(*client_secret_), std::move(scope_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string OAuthClientCredentialsRequestBuilder::TryBuild(optional<OAuthClientCredentialsRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string OAuthClientCredentialsRequest::TryFromJSON(yyjson_val *obj, OAuthClientCredentialsRequestBuilder &builder) {
	try {
		auto grant_type_val = yyjson_obj_get(obj, "grant_type");
		if (!grant_type_val) {
			throw InvalidInputException("OAuthClientCredentialsRequest required property 'grant_type' is missing");
		} else {
			string grant_type;
			if (yyjson_is_str(grant_type_val)) {
				grant_type = yyjson_get_str(grant_type_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthClientCredentialsRequest property 'grant_type' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(grant_type_val)));
			}
			builder.SetGrantType(std::move(grant_type));
		}
		auto client_id_val = yyjson_obj_get(obj, "client_id");
		if (!client_id_val) {
			throw InvalidInputException("OAuthClientCredentialsRequest required property 'client_id' is missing");
		} else {
			string client_id;
			if (yyjson_is_str(client_id_val)) {
				client_id = yyjson_get_str(client_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthClientCredentialsRequest property 'client_id' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(client_id_val)));
			}
			builder.SetClientId(std::move(client_id));
		}
		auto client_secret_val = yyjson_obj_get(obj, "client_secret");
		if (!client_secret_val) {
			throw InvalidInputException("OAuthClientCredentialsRequest required property 'client_secret' is missing");
		} else {
			string client_secret;
			if (yyjson_is_str(client_secret_val)) {
				client_secret = yyjson_get_str(client_secret_val);
			} else {
				throw InvalidInputException(StringUtil::Format("OAuthClientCredentialsRequest property 'client_secret' "
				                                               "is not of type 'string', found '%s' instead",
				                                               yyjson_get_type_desc(client_secret_val)));
			}
			builder.SetClientSecret(std::move(client_secret));
		}
		auto scope_val = yyjson_obj_get(obj, "scope");
		if (scope_val) {
			string scope;
			if (yyjson_is_str(scope_val)) {
				scope = yyjson_get_str(scope_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthClientCredentialsRequest property 'scope' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(scope_val)));
			}
			builder.SetScope(std::move(scope));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthClientCredentialsRequest OAuthClientCredentialsRequest::FromJSON(yyjson_val *obj) {
	OAuthClientCredentialsRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

OAuthClientCredentialsRequest OAuthClientCredentialsRequest::Copy() const {
	OAuthClientCredentialsRequestBuilder builder;
	string grant_type_tmp;
	grant_type_tmp = grant_type;
	builder.SetGrantType(std::move(grant_type_tmp));
	string client_id_tmp;
	client_id_tmp = client_id;
	builder.SetClientId(std::move(client_id_tmp));
	string client_secret_tmp;
	client_secret_tmp = client_secret;
	builder.SetClientSecret(std::move(client_secret_tmp));
	optional<string> scope_tmp;
	if (scope.has_value()) {
		scope_tmp.emplace();
		(*scope_tmp) = (*scope);
	}
	if (scope_tmp.has_value()) {
		builder.SetScope(std::move((*scope_tmp)));
	}
	return builder.Build();
}

string OAuthClientCredentialsRequest::Validate() const {
	string error;
	if (!StringUtil::CIEquals(grant_type, "client_credentials")) {
		return StringUtil::Format(
		    "OAuthClientCredentialsRequest property 'grant_type' must be one of [client_credentials], not %s",
		    grant_type);
	}
	return "";
}

void OAuthClientCredentialsRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: grant_type
	yyjson_mut_obj_add_strcpy(doc, obj, "grant_type", grant_type.c_str());

	// Serialize: client_id
	yyjson_mut_obj_add_strcpy(doc, obj, "client_id", client_id.c_str());

	// Serialize: client_secret
	yyjson_mut_obj_add_strcpy(doc, obj, "client_secret", client_secret.c_str());

	// Serialize: scope
	if (scope.has_value()) {
		auto &scope_value = *scope;
		yyjson_mut_obj_add_strcpy(doc, obj, "scope", scope_value.c_str());
	}
}

yyjson_mut_val *OAuthClientCredentialsRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
