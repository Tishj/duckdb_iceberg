
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

class OAuthTokenResponseBuilder;

class OAuthTokenResponse {
public:
	OAuthTokenResponse(const OAuthTokenResponse &);
	OAuthTokenResponse &operator=(const OAuthTokenResponse &) = delete;
	OAuthTokenResponse(OAuthTokenResponse &&);
	OAuthTokenResponse &operator=(OAuthTokenResponse &&) = delete;

private:
	friend class OAuthTokenResponseBuilder;
	OAuthTokenResponse(string access_token_p, string token_type_p, optional<int32_t> expires_in_p,
	                   optional<TokenType> issued_token_type_p, optional<string> refresh_token_p,
	                   optional<string> scope_p);

public:
	// Deserialization
	static OAuthTokenResponse FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, OAuthTokenResponseBuilder &builder);
	string Validate() const;

	// Copy
	OAuthTokenResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string access_token;
	const string token_type;
	const optional<int32_t> expires_in;
	const optional<TokenType> issued_token_type;
	const optional<string> refresh_token;
	const optional<string> scope;
};

class OAuthTokenResponseBuilder {
public:
	OAuthTokenResponseBuilder();
	OAuthTokenResponseBuilder &SetAccessToken(string value);
	OAuthTokenResponseBuilder &SetTokenType(string value);
	OAuthTokenResponseBuilder &SetExpiresIn(int32_t value);
	OAuthTokenResponseBuilder &SetIssuedTokenType(TokenType value);
	OAuthTokenResponseBuilder &SetRefreshToken(string value);
	OAuthTokenResponseBuilder &SetScope(string value);
	string TryBuild(optional<OAuthTokenResponse> &result);
	OAuthTokenResponse Build();

private:
	optional<string> access_token_;
	optional<string> token_type_;
	optional<int32_t> expires_in_;
	optional<TokenType> issued_token_type_;
	optional<string> refresh_token_;
	optional<string> scope_;
	bool has_access_token_ = false;
	bool has_token_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
