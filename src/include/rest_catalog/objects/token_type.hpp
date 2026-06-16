
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

class TokenType {
public:
	TokenType(const TokenType &);
	TokenType &operator=(const TokenType &) = delete;
	TokenType(TokenType &&);
	TokenType &operator=(TokenType &&) = delete;
	TokenType(string value_p);

public:
	// Deserialization
	static TokenType FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<TokenType> &result);
	string Validate() const;

	// Copy
	TokenType Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string value;
};

} // namespace rest_api_objects
} // namespace duckdb
