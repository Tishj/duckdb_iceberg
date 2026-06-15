
#include "rest_catalog/objects/oauth_token_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

OAuthTokenRequest::OAuthTokenRequest() {
}

OAuthTokenRequestBuilder::OAuthTokenRequestBuilder() {
}

OAuthTokenRequestBuilder &
OAuthTokenRequestBuilder::SetOauthClientCredentialsRequest(OAuthClientCredentialsRequest value) {
	result_.oauth_client_credentials_request = std::move(value);
	return *this;
}

OAuthTokenRequestBuilder &OAuthTokenRequestBuilder::SetOauthTokenExchangeRequest(OAuthTokenExchangeRequest value) {
	result_.oauth_token_exchange_request = std::move(value);
	return *this;
}

string OAuthTokenRequestBuilder::TryBuild(OAuthTokenRequest &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

OAuthTokenRequest OAuthTokenRequestBuilder::Build() {
	OAuthTokenRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

OAuthTokenRequest OAuthTokenRequest::FromJSON(yyjson_val *obj) {
	OAuthTokenRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

OAuthTokenRequest OAuthTokenRequest::Copy() const {
	OAuthTokenRequest res;
	if (oauth_client_credentials_request.has_value()) {
		res.oauth_client_credentials_request.emplace();
		(*res.oauth_client_credentials_request) = (*oauth_client_credentials_request).Copy();
	}
	if (oauth_token_exchange_request.has_value()) {
		res.oauth_token_exchange_request.emplace();
		(*res.oauth_token_exchange_request) = (*oauth_token_exchange_request).Copy();
	}
	return res;
}

string OAuthTokenRequest::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (oauth_client_credentials_request.has_value()) {
		matched_any_of_variants++;
		error = oauth_client_credentials_request->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (oauth_token_exchange_request.has_value()) {
		matched_any_of_variants++;
		error = oauth_token_exchange_request->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "OAuthTokenRequest must have at least one anyOf variant set";
	}
	return "";
}

string OAuthTokenRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	oauth_client_credentials_request.emplace();
	error = oauth_client_credentials_request->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		oauth_client_credentials_request = nullopt;
	}
	oauth_token_exchange_request.emplace();
	error = oauth_token_exchange_request->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		oauth_token_exchange_request = nullopt;
	}
	if (!(oauth_client_credentials_request.has_value()) && !(oauth_token_exchange_request.has_value())) {
		return "OAuthTokenRequest failed to parse, none of the anyOf candidates matched";
	}
	return Validate();
}

void OAuthTokenRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (oauth_client_credentials_request.has_value()) {
		oauth_client_credentials_request->PopulateJSON(doc, obj);
	} else if (oauth_token_exchange_request.has_value()) {
		oauth_token_exchange_request->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *OAuthTokenRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
