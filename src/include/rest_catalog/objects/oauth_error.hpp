
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class OAuthError {
public:
	OAuthError();
	OAuthError(const OAuthError &) = delete;
	OAuthError &operator=(const OAuthError &) = delete;
	OAuthError(OAuthError &&) = default;
	OAuthError &operator=(OAuthError &&) = default;

public:
	// Deserialization
	static OAuthError FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	OAuthError Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string _error;
	optional<string> error_description;
	optional<string> error_uri;
};

class OAuthErrorBuilder {
public:
	OAuthErrorBuilder();
	OAuthErrorBuilder &SetError(string value);
	OAuthErrorBuilder &SetErrorDescription(string value);
	OAuthErrorBuilder &SetErrorUri(string value);
	string TryBuild(OAuthError &result);
	OAuthError Build();

private:
	OAuthError result_;
	bool has__error_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
