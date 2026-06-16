
#include "rest_catalog/objects/oauth_token_response.hpp"

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

OAuthTokenResponse::OAuthTokenResponse(string access_token_p, string token_type_p, optional<int32_t> expires_in_p,
                                       optional<TokenType> issued_token_type_p, optional<string> refresh_token_p,
                                       optional<string> scope_p)
    : access_token(std::move(access_token_p)), token_type(std::move(token_type_p)), expires_in(std::move(expires_in_p)),
      issued_token_type(std::move(issued_token_type_p)), refresh_token(std::move(refresh_token_p)),
      scope(std::move(scope_p)) {
}

OAuthTokenResponseBuilder::OAuthTokenResponseBuilder() {
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetAccessToken(string value) {
	access_token_.emplace(std::move(value));
	has_access_token_ = true;
	return *this;
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetTokenType(string value) {
	token_type_.emplace(std::move(value));
	has_token_type_ = true;
	return *this;
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetExpiresIn(int32_t value) {
	expires_in_.emplace(std::move(value));
	return *this;
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetIssuedTokenType(TokenType value) {
	issued_token_type_.emplace(std::move(value));
	return *this;
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetRefreshToken(string value) {
	refresh_token_.emplace(std::move(value));
	return *this;
}

OAuthTokenResponseBuilder &OAuthTokenResponseBuilder::SetScope(string value) {
	scope_.emplace(std::move(value));
	return *this;
}

OAuthTokenResponse OAuthTokenResponseBuilder::Build() {
	if (!has_access_token_) {
		throw InvalidInputException("OAuthTokenResponse required property 'access_token' is missing");
	}
	if (!has_token_type_) {
		throw InvalidInputException("OAuthTokenResponse required property 'token_type' is missing");
	}
	auto result = OAuthTokenResponse(std::move(*access_token_), std::move(*token_type_), std::move(expires_in_),
	                                 std::move(issued_token_type_), std::move(refresh_token_), std::move(scope_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string OAuthTokenResponseBuilder::TryBuild(optional<OAuthTokenResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string OAuthTokenResponse::TryFromJSON(yyjson_val *obj, OAuthTokenResponseBuilder &builder) {
	try {
		auto access_token_val = yyjson_obj_get(obj, "access_token");
		if (!access_token_val) {
			throw InvalidInputException("OAuthTokenResponse required property 'access_token' is missing");
		} else {
			string access_token;
			if (yyjson_is_str(access_token_val)) {
				access_token = yyjson_get_str(access_token_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthTokenResponse property 'access_token' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(access_token_val)));
			}
			builder.SetAccessToken(std::move(access_token));
		}
		auto token_type_val = yyjson_obj_get(obj, "token_type");
		if (!token_type_val) {
			throw InvalidInputException("OAuthTokenResponse required property 'token_type' is missing");
		} else {
			string token_type;
			if (yyjson_is_str(token_type_val)) {
				token_type = yyjson_get_str(token_type_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthTokenResponse property 'token_type' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(token_type_val)));
			}
			builder.SetTokenType(std::move(token_type));
		}
		auto expires_in_val = yyjson_obj_get(obj, "expires_in");
		if (expires_in_val) {
			int32_t expires_in;
			if (yyjson_is_int(expires_in_val)) {
				expires_in = yyjson_get_int(expires_in_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthTokenResponse property 'expires_in' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(expires_in_val)));
			}
			builder.SetExpiresIn(std::move(expires_in));
		}
		auto issued_token_type_val = yyjson_obj_get(obj, "issued_token_type");
		if (issued_token_type_val) {
			builder.SetIssuedTokenType(TokenType::FromJSON(issued_token_type_val));
		}
		auto refresh_token_val = yyjson_obj_get(obj, "refresh_token");
		if (refresh_token_val) {
			string refresh_token;
			if (yyjson_is_str(refresh_token_val)) {
				refresh_token = yyjson_get_str(refresh_token_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthTokenResponse property 'refresh_token' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(refresh_token_val)));
			}
			builder.SetRefreshToken(std::move(refresh_token));
		}
		auto scope_val = yyjson_obj_get(obj, "scope");
		if (scope_val) {
			string scope;
			if (yyjson_is_str(scope_val)) {
				scope = yyjson_get_str(scope_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthTokenResponse property 'scope' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(scope_val)));
			}
			builder.SetScope(std::move(scope));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthTokenResponse OAuthTokenResponse::FromJSON(yyjson_val *obj) {
	OAuthTokenResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

OAuthTokenResponse OAuthTokenResponse::Copy() const {
	OAuthTokenResponseBuilder builder;
	string access_token_tmp;
	access_token_tmp = access_token;
	builder.SetAccessToken(std::move(access_token_tmp));
	string token_type_tmp;
	token_type_tmp = token_type;
	builder.SetTokenType(std::move(token_type_tmp));
	optional<int32_t> expires_in_tmp;
	if (expires_in.has_value()) {
		expires_in_tmp.emplace();
		(*expires_in_tmp) = (*expires_in);
	}
	if (expires_in_tmp.has_value()) {
		builder.SetExpiresIn(std::move((*expires_in_tmp)));
	}
	optional<TokenType> issued_token_type_tmp;
	if (issued_token_type.has_value()) {
		issued_token_type_tmp.emplace((*issued_token_type).Copy());
	}
	if (issued_token_type_tmp.has_value()) {
		builder.SetIssuedTokenType(std::move(*issued_token_type_tmp));
	}
	optional<string> refresh_token_tmp;
	if (refresh_token.has_value()) {
		refresh_token_tmp.emplace();
		(*refresh_token_tmp) = (*refresh_token);
	}
	if (refresh_token_tmp.has_value()) {
		builder.SetRefreshToken(std::move((*refresh_token_tmp)));
	}
	optional<string> scope_tmp;
	if (scope.has_value()) {
		scope_tmp.emplace();
		(*scope_tmp) = (*scope);
	}
	if (scope_tmp.has_value()) {
		builder.SetScope(std::move((*scope_tmp)));
	}
	return builder.Build();
}

string OAuthTokenResponse::Validate() const {
	string error;
	if (!StringUtil::CIEquals(token_type, "bearer") && !StringUtil::CIEquals(token_type, "mac") &&
	    !StringUtil::CIEquals(token_type, "N_A")) {
		return StringUtil::Format("OAuthTokenResponse property 'token_type' must be one of [bearer, mac, N_A], not %s",
		                          token_type);
	}
	if (issued_token_type.has_value()) {
		error = (*issued_token_type).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void OAuthTokenResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: access_token
	yyjson_mut_obj_add_strcpy(doc, obj, "access_token", access_token.c_str());

	// Serialize: token_type
	yyjson_mut_obj_add_strcpy(doc, obj, "token_type", token_type.c_str());

	// Serialize: expires_in
	if (expires_in.has_value()) {
		auto &expires_in_value = *expires_in;
		yyjson_mut_obj_add_int(doc, obj, "expires_in", expires_in_value);
	}

	// Serialize: issued_token_type
	if (issued_token_type.has_value()) {
		auto &issued_token_type_value = *issued_token_type;
		yyjson_mut_val *issued_token_type_value_val = issued_token_type_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "issued_token_type", issued_token_type_value_val);
	}

	// Serialize: refresh_token
	if (refresh_token.has_value()) {
		auto &refresh_token_value = *refresh_token;
		yyjson_mut_obj_add_strcpy(doc, obj, "refresh_token", refresh_token_value.c_str());
	}

	// Serialize: scope
	if (scope.has_value()) {
		auto &scope_value = *scope;
		yyjson_mut_obj_add_strcpy(doc, obj, "scope", scope_value.c_str());
	}
}

yyjson_mut_val *OAuthTokenResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
