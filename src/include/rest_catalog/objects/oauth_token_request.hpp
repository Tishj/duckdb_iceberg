
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/oauth_client_credentials_request.hpp"
#include "rest_catalog/objects/oauth_token_exchange_request.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class OAuthTokenRequestBuilder;

class OAuthTokenRequest {
public:
	OAuthTokenRequest(const OAuthTokenRequest &);
	OAuthTokenRequest &operator=(const OAuthTokenRequest &) = delete;
	OAuthTokenRequest(OAuthTokenRequest &&);
	OAuthTokenRequest &operator=(OAuthTokenRequest &&) = delete;

private:
	friend class OAuthTokenRequestBuilder;
	OAuthTokenRequest(optional<OAuthClientCredentialsRequest> oauth_client_credentials_request_p,
	                  optional<OAuthTokenExchangeRequest> oauth_token_exchange_request_p);

public:
	// Deserialization
	static OAuthTokenRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, OAuthTokenRequestBuilder &builder);
	string Validate() const;

	// Copy
	OAuthTokenRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<OAuthClientCredentialsRequest> oauth_client_credentials_request;
	const optional<OAuthTokenExchangeRequest> oauth_token_exchange_request;
};

class OAuthTokenRequestBuilder {
public:
	OAuthTokenRequestBuilder();
	OAuthTokenRequestBuilder &SetOauthClientCredentialsRequest(OAuthClientCredentialsRequest value);
	OAuthTokenRequestBuilder &SetOauthTokenExchangeRequest(OAuthTokenExchangeRequest value);
	string TryBuild(optional<OAuthTokenRequest> &result);
	OAuthTokenRequest Build();

private:
	optional<OAuthClientCredentialsRequest> oauth_client_credentials_request_;
	optional<OAuthTokenExchangeRequest> oauth_token_exchange_request_;
};

} // namespace rest_api_objects
} // namespace duckdb
