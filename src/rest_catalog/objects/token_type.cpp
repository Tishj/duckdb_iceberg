
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

TokenType TokenType::FromJSON(yyjson_val *obj) {
	string value;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "TokenType property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return TokenType(std::move(value));
}

string TokenType::TryFromJSON(yyjson_val *obj, optional<TokenType> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TokenType TokenType::Copy() const {
	string value_tmp;
	value_tmp = value;
	return TokenType(std::move(value_tmp));
}

string TokenType::Validate() const {
	string error;
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
	return "";
}

yyjson_mut_val *TokenType::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
