
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/token_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class OAuthTokenExchangeRequest {
public:
	OAuthTokenExchangeRequest();
	OAuthTokenExchangeRequest(const OAuthTokenExchangeRequest &) = delete;
	OAuthTokenExchangeRequest &operator=(const OAuthTokenExchangeRequest &) = delete;
	OAuthTokenExchangeRequest(OAuthTokenExchangeRequest &&) = default;
	OAuthTokenExchangeRequest &operator=(OAuthTokenExchangeRequest &&) = default;

public:
	// Deserialization
	static OAuthTokenExchangeRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	OAuthTokenExchangeRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string grant_type;
	string subject_token;
	TokenType subject_token_type;
	optional<string> scope;
	optional<TokenType> requested_token_type;
	optional<string> actor_token;
	optional<TokenType> actor_token_type;
};

class OAuthTokenExchangeRequestBuilder {
public:
	OAuthTokenExchangeRequestBuilder();
	OAuthTokenExchangeRequestBuilder &SetGrantType(string value);
	OAuthTokenExchangeRequestBuilder &SetSubjectToken(string value);
	OAuthTokenExchangeRequestBuilder &SetSubjectTokenType(TokenType value);
	OAuthTokenExchangeRequestBuilder &SetScope(string value);
	OAuthTokenExchangeRequestBuilder &SetRequestedTokenType(TokenType value);
	OAuthTokenExchangeRequestBuilder &SetActorToken(string value);
	OAuthTokenExchangeRequestBuilder &SetActorTokenType(TokenType value);
	string TryBuild(OAuthTokenExchangeRequest &result);
	OAuthTokenExchangeRequest Build();

private:
	OAuthTokenExchangeRequest result_;
	bool has_grant_type_ = false;
	bool has_subject_token_ = false;
	bool has_subject_token_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
