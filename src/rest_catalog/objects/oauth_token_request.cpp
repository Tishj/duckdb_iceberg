
#include "rest_catalog/objects/oauth_token_request.hpp"

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

OAuthTokenRequest::OAuthTokenRequest(optional<OAuthClientCredentialsRequest> oauth_client_credentials_request_p,
                                     optional<OAuthTokenExchangeRequest> oauth_token_exchange_request_p)
    : oauth_client_credentials_request(std::move(oauth_client_credentials_request_p)),
      oauth_token_exchange_request(std::move(oauth_token_exchange_request_p)) {
}
OAuthTokenRequest::OAuthTokenRequest(const OAuthTokenRequest &other)
    : oauth_client_credentials_request(
          (other.oauth_client_credentials_request.has_value()
               ? optional<OAuthClientCredentialsRequest>((*other.oauth_client_credentials_request).Copy())
               : optional<OAuthClientCredentialsRequest>())),
      oauth_token_exchange_request(
          (other.oauth_token_exchange_request.has_value()
               ? optional<OAuthTokenExchangeRequest>((*other.oauth_token_exchange_request).Copy())
               : optional<OAuthTokenExchangeRequest>())) {
}
OAuthTokenRequest::OAuthTokenRequest(OAuthTokenRequest &&other)
    : OAuthTokenRequest(static_cast<const OAuthTokenRequest &>(other)) {
}

OAuthTokenRequestBuilder::OAuthTokenRequestBuilder() {
}

OAuthTokenRequestBuilder &
OAuthTokenRequestBuilder::SetOauthClientCredentialsRequest(OAuthClientCredentialsRequest value) {
	oauth_client_credentials_request_.emplace(std::move(value));
	return *this;
}

OAuthTokenRequestBuilder &OAuthTokenRequestBuilder::SetOauthTokenExchangeRequest(OAuthTokenExchangeRequest value) {
	oauth_token_exchange_request_.emplace(std::move(value));
	return *this;
}

OAuthTokenRequest OAuthTokenRequestBuilder::Build() {
	auto result =
	    OAuthTokenRequest(std::move(oauth_client_credentials_request_), std::move(oauth_token_exchange_request_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string OAuthTokenRequestBuilder::TryBuild(optional<OAuthTokenRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string OAuthTokenRequest::TryFromJSON(yyjson_val *obj, OAuthTokenRequestBuilder &builder) {
	try {
		int matched_any_of_variants = 0;
		try {
			builder.SetOauthClientCredentialsRequest(OAuthClientCredentialsRequest::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetOauthTokenExchangeRequest(OAuthTokenExchangeRequest::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		if (matched_any_of_variants == 0) {
			throw InvalidInputException("OAuthTokenRequest failed to parse, none of the anyOf candidates matched");
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthTokenRequest OAuthTokenRequest::FromJSON(yyjson_val *obj) {
	OAuthTokenRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

OAuthTokenRequest OAuthTokenRequest::Copy() const {
	return OAuthTokenRequest(*this);
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
