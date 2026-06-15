
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class OAuthClientCredentialsRequestBuilder;

class OAuthClientCredentialsRequest {
public:
	OAuthClientCredentialsRequest(const OAuthClientCredentialsRequest &) = delete;
	OAuthClientCredentialsRequest &operator=(const OAuthClientCredentialsRequest &) = delete;
	OAuthClientCredentialsRequest(OAuthClientCredentialsRequest &&) = default;
	OAuthClientCredentialsRequest &operator=(OAuthClientCredentialsRequest &&) = delete;

private:
	friend class OAuthClientCredentialsRequestBuilder;
	OAuthClientCredentialsRequest(string grant_type_p, string client_id_p, string client_secret_p,
	                              optional<string> scope_p);

public:
	// Deserialization
	static OAuthClientCredentialsRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<OAuthClientCredentialsRequest> &result);
	string Validate() const;

	// Copy
	OAuthClientCredentialsRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string grant_type;
	string client_id;
	string client_secret;
	optional<string> scope;
};

class OAuthClientCredentialsRequestBuilder {
public:
	OAuthClientCredentialsRequestBuilder();
	OAuthClientCredentialsRequestBuilder &SetGrantType(string value);
	OAuthClientCredentialsRequestBuilder &SetClientId(string value);
	OAuthClientCredentialsRequestBuilder &SetClientSecret(string value);
	OAuthClientCredentialsRequestBuilder &SetScope(string value);
	string TryBuild(optional<OAuthClientCredentialsRequest> &result);
	OAuthClientCredentialsRequest Build();

private:
	optional<string> grant_type_;
	optional<string> client_id_;
	optional<string> client_secret_;
	optional<string> scope_;
	bool has_grant_type_ = false;
	bool has_client_id_ = false;
	bool has_client_secret_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
