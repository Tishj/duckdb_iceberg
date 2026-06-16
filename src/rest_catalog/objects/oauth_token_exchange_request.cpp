
#include "rest_catalog/objects/oauth_token_exchange_request.hpp"

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

OAuthTokenExchangeRequest::OAuthTokenExchangeRequest(string grant_type_p, string subject_token_p,
                                                     TokenType subject_token_type_p, optional<string> scope_p,
                                                     optional<TokenType> requested_token_type_p,
                                                     optional<string> actor_token_p,
                                                     optional<TokenType> actor_token_type_p)
    : grant_type(std::move(grant_type_p)), subject_token(std::move(subject_token_p)),
      subject_token_type(std::move(subject_token_type_p)), scope(std::move(scope_p)),
      requested_token_type(std::move(requested_token_type_p)), actor_token(std::move(actor_token_p)),
      actor_token_type(std::move(actor_token_type_p)) {
}

OAuthTokenExchangeRequestBuilder::OAuthTokenExchangeRequestBuilder() {
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetGrantType(string value) {
	grant_type_.emplace(std::move(value));
	has_grant_type_ = true;
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetSubjectToken(string value) {
	subject_token_.emplace(std::move(value));
	has_subject_token_ = true;
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetSubjectTokenType(TokenType value) {
	subject_token_type_.emplace(std::move(value));
	has_subject_token_type_ = true;
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetScope(string value) {
	scope_.emplace(std::move(value));
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetRequestedTokenType(TokenType value) {
	requested_token_type_.emplace(std::move(value));
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetActorToken(string value) {
	actor_token_.emplace(std::move(value));
	return *this;
}

OAuthTokenExchangeRequestBuilder &OAuthTokenExchangeRequestBuilder::SetActorTokenType(TokenType value) {
	actor_token_type_.emplace(std::move(value));
	return *this;
}

OAuthTokenExchangeRequest OAuthTokenExchangeRequestBuilder::Build() {
	if (!has_grant_type_) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'grant_type' is missing");
	}
	if (!has_subject_token_) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'subject_token' is missing");
	}
	if (!has_subject_token_type_) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'subject_token_type' is missing");
	}
	auto result = OAuthTokenExchangeRequest(
	    std::move(*grant_type_), std::move(*subject_token_), std::move(*subject_token_type_), std::move(scope_),
	    std::move(requested_token_type_), std::move(actor_token_), std::move(actor_token_type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string OAuthTokenExchangeRequestBuilder::TryBuild(optional<OAuthTokenExchangeRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthTokenExchangeRequest OAuthTokenExchangeRequest::FromJSON(yyjson_val *obj) {
	OAuthTokenExchangeRequestBuilder builder;
	auto grant_type_val = yyjson_obj_get(obj, "grant_type");
	if (!grant_type_val) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'grant_type' is missing");
	} else {
		string grant_type;
		if (yyjson_is_str(grant_type_val)) {
			grant_type = yyjson_get_str(grant_type_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "OAuthTokenExchangeRequest property 'grant_type' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(grant_type_val)));
		}
		builder.SetGrantType(std::move(grant_type));
	}
	auto subject_token_val = yyjson_obj_get(obj, "subject_token");
	if (!subject_token_val) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'subject_token' is missing");
	} else {
		string subject_token;
		if (yyjson_is_str(subject_token_val)) {
			subject_token = yyjson_get_str(subject_token_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "OAuthTokenExchangeRequest property 'subject_token' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(subject_token_val)));
		}
		builder.SetSubjectToken(std::move(subject_token));
	}
	auto subject_token_type_val = yyjson_obj_get(obj, "subject_token_type");
	if (!subject_token_type_val) {
		throw InvalidInputException("OAuthTokenExchangeRequest required property 'subject_token_type' is missing");
	} else {
		builder.SetSubjectTokenType(TokenType::FromJSON(subject_token_type_val));
	}
	auto scope_val = yyjson_obj_get(obj, "scope");
	if (scope_val) {
		string scope;
		if (yyjson_is_str(scope_val)) {
			scope = yyjson_get_str(scope_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "OAuthTokenExchangeRequest property 'scope' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(scope_val)));
		}
		builder.SetScope(std::move(scope));
	}
	auto requested_token_type_val = yyjson_obj_get(obj, "requested_token_type");
	if (requested_token_type_val) {
		builder.SetRequestedTokenType(TokenType::FromJSON(requested_token_type_val));
	}
	auto actor_token_val = yyjson_obj_get(obj, "actor_token");
	if (actor_token_val) {
		string actor_token;
		if (yyjson_is_str(actor_token_val)) {
			actor_token = yyjson_get_str(actor_token_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "OAuthTokenExchangeRequest property 'actor_token' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(actor_token_val)));
		}
		builder.SetActorToken(std::move(actor_token));
	}
	auto actor_token_type_val = yyjson_obj_get(obj, "actor_token_type");
	if (actor_token_type_val) {
		builder.SetActorTokenType(TokenType::FromJSON(actor_token_type_val));
	}
	return builder.Build();
}

string OAuthTokenExchangeRequest::TryFromJSON(yyjson_val *obj, optional<OAuthTokenExchangeRequest> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthTokenExchangeRequest OAuthTokenExchangeRequest::Copy() const {
	OAuthTokenExchangeRequestBuilder builder;
	string grant_type_tmp;
	grant_type_tmp = grant_type;
	builder.SetGrantType(std::move(grant_type_tmp));
	string subject_token_tmp;
	subject_token_tmp = subject_token;
	builder.SetSubjectToken(std::move(subject_token_tmp));
	auto subject_token_type_tmp = subject_token_type.Copy();
	builder.SetSubjectTokenType(std::move(subject_token_type_tmp));
	optional<string> scope_tmp;
	if (scope.has_value()) {
		scope_tmp.emplace();
		(*scope_tmp) = (*scope);
	}
	if (scope_tmp.has_value()) {
		builder.SetScope(std::move((*scope_tmp)));
	}
	optional<TokenType> requested_token_type_tmp;
	if (requested_token_type.has_value()) {
		requested_token_type_tmp.emplace((*requested_token_type).Copy());
	}
	if (requested_token_type_tmp.has_value()) {
		builder.SetRequestedTokenType(std::move(*requested_token_type_tmp));
	}
	optional<string> actor_token_tmp;
	if (actor_token.has_value()) {
		actor_token_tmp.emplace();
		(*actor_token_tmp) = (*actor_token);
	}
	if (actor_token_tmp.has_value()) {
		builder.SetActorToken(std::move((*actor_token_tmp)));
	}
	optional<TokenType> actor_token_type_tmp;
	if (actor_token_type.has_value()) {
		actor_token_type_tmp.emplace((*actor_token_type).Copy());
	}
	if (actor_token_type_tmp.has_value()) {
		builder.SetActorTokenType(std::move(*actor_token_type_tmp));
	}
	return builder.Build();
}

string OAuthTokenExchangeRequest::Validate() const {
	string error;
	if (!StringUtil::CIEquals(grant_type, "urn:ietf:params:oauth:grant-type:token-exchange")) {
		return StringUtil::Format("OAuthTokenExchangeRequest property 'grant_type' must be one of "
		                          "[urn:ietf:params:oauth:grant-type:token-exchange], not %s",
		                          grant_type);
	}
	error = subject_token_type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (requested_token_type.has_value()) {
		error = (*requested_token_type).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (actor_token_type.has_value()) {
		error = (*actor_token_type).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void OAuthTokenExchangeRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: grant_type
	yyjson_mut_obj_add_strcpy(doc, obj, "grant_type", grant_type.c_str());

	// Serialize: subject_token
	yyjson_mut_obj_add_strcpy(doc, obj, "subject_token", subject_token.c_str());

	// Serialize: subject_token_type
	yyjson_mut_val *subject_token_type_val = subject_token_type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "subject_token_type", subject_token_type_val);

	// Serialize: scope
	if (scope.has_value()) {
		auto &scope_value = *scope;
		yyjson_mut_obj_add_strcpy(doc, obj, "scope", scope_value.c_str());
	}

	// Serialize: requested_token_type
	if (requested_token_type.has_value()) {
		auto &requested_token_type_value = *requested_token_type;
		yyjson_mut_val *requested_token_type_value_val = requested_token_type_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "requested_token_type", requested_token_type_value_val);
	}

	// Serialize: actor_token
	if (actor_token.has_value()) {
		auto &actor_token_value = *actor_token;
		yyjson_mut_obj_add_strcpy(doc, obj, "actor_token", actor_token_value.c_str());
	}

	// Serialize: actor_token_type
	if (actor_token_type.has_value()) {
		auto &actor_token_type_value = *actor_token_type;
		yyjson_mut_val *actor_token_type_value_val = actor_token_type_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "actor_token_type", actor_token_type_value_val);
	}
}

yyjson_mut_val *OAuthTokenExchangeRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
