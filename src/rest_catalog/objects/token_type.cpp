
#include "rest_catalog/objects/token_type.hpp"

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

TokenType::TokenType(string value_p) : value(std::move(value_p)) {
}
TokenType::TokenType(const TokenType &other) : value(other.value) {
}
TokenType::TokenType(TokenType &&other) : TokenType(static_cast<const TokenType &>(other)) {
}

optional<string> TokenType::TryFromJSON(yyjson_val *obj, optional<TokenType> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "TokenType property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(TokenType(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TokenType TokenType::FromJSON(yyjson_val *obj) {
	optional<TokenType> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

TokenType TokenType::Copy() const {
	return TokenType(*this);
}

optional<string> TokenType::Validate() const {
	optional<string> error;
	if (!StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:access_token") &&
	    !StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:refresh_token") &&
	    !StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:id_token") &&
	    !StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:saml1") &&
	    !StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:saml2") &&
	    !StringUtil::CIEquals(value, "urn:ietf:params:oauth:token-type:jwt")) {
		return StringUtil::Format(
		    "TokenType property 'value' must be one of [urn:ietf:params:oauth:token-type:access_token, "
		    "urn:ietf:params:oauth:token-type:refresh_token, urn:ietf:params:oauth:token-type:id_token, "
		    "urn:ietf:params:oauth:token-type:saml1, urn:ietf:params:oauth:token-type:saml2, "
		    "urn:ietf:params:oauth:token-type:jwt], not %s",
		    value);
	}
	return nullopt;
}

yyjson_mut_val *TokenType::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
