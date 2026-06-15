
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/token_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class OAuthTokenExchangeRequestBuilder;

class OAuthTokenExchangeRequest {
public:
	OAuthTokenExchangeRequest(const OAuthTokenExchangeRequest &) = delete;
	OAuthTokenExchangeRequest &operator=(const OAuthTokenExchangeRequest &) = delete;
	OAuthTokenExchangeRequest(OAuthTokenExchangeRequest &&) = default;
	OAuthTokenExchangeRequest &operator=(OAuthTokenExchangeRequest &&) = delete;

private:
	friend class OAuthTokenExchangeRequestBuilder;
	OAuthTokenExchangeRequest(string grant_type_p, string subject_token_p, TokenType subject_token_type_p,
	                          optional<string> scope_p, optional<TokenType> requested_token_type_p,
	                          optional<string> actor_token_p, optional<TokenType> actor_token_type_p);

public:
	// Deserialization
	static OAuthTokenExchangeRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<OAuthTokenExchangeRequest> &result);
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
	string TryBuild(optional<OAuthTokenExchangeRequest> &result);
	OAuthTokenExchangeRequest Build();

private:
	optional<string> grant_type_;
	optional<string> subject_token_;
	optional<TokenType> subject_token_type_;
	optional<string> scope_;
	optional<TokenType> requested_token_type_;
	optional<string> actor_token_;
	optional<TokenType> actor_token_type_;
	bool has_grant_type_ = false;
	bool has_subject_token_ = false;
	bool has_subject_token_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
