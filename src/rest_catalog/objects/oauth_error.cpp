
#include "rest_catalog/objects/oauth_error.hpp"

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

OAuthError::OAuthError(string _error_p, optional<string> error_description_p, optional<string> error_uri_p)
    : _error(std::move(_error_p)), error_description(std::move(error_description_p)),
      error_uri(std::move(error_uri_p)) {
}
OAuthError::OAuthError(const OAuthError &other)
    : _error(other._error),
      error_description(
          (other.error_description.has_value() ? optional<string>((*other.error_description)) : optional<string>())),
      error_uri((other.error_uri.has_value() ? optional<string>((*other.error_uri)) : optional<string>())) {
}
OAuthError::OAuthError(OAuthError &&other) : OAuthError(static_cast<const OAuthError &>(other)) {
}

OAuthErrorBuilder::OAuthErrorBuilder() {
}

OAuthErrorBuilder &OAuthErrorBuilder::SetError(string value) {
	_error_.emplace(std::move(value));
	has__error_ = true;
	return *this;
}

OAuthErrorBuilder &OAuthErrorBuilder::SetErrorDescription(string value) {
	error_description_.emplace(std::move(value));
	return *this;
}

OAuthErrorBuilder &OAuthErrorBuilder::SetErrorUri(string value) {
	error_uri_.emplace(std::move(value));
	return *this;
}

OAuthError OAuthErrorBuilder::Build() {
	if (!has__error_) {
		throw InvalidInputException("OAuthError required property 'error' is missing");
	}
	auto result = OAuthError(std::move(*_error_), std::move(error_description_), std::move(error_uri_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string OAuthErrorBuilder::TryBuild(optional<OAuthError> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string OAuthError::TryFromJSON(yyjson_val *obj, OAuthErrorBuilder &builder) {
	try {
		auto _error_val = yyjson_obj_get(obj, "error");
		if (!_error_val) {
			throw InvalidInputException("OAuthError required property 'error' is missing");
		} else {
			string _error;
			if (yyjson_is_str(_error_val)) {
				_error = yyjson_get_str(_error_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("OAuthError property '_error' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(_error_val)));
			}
			builder.SetError(std::move(_error));
		}
		auto error_description_val = yyjson_obj_get(obj, "error_description");
		if (error_description_val) {
			string error_description;
			if (yyjson_is_str(error_description_val)) {
				error_description = yyjson_get_str(error_description_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "OAuthError property 'error_description' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(error_description_val)));
			}
			builder.SetErrorDescription(std::move(error_description));
		}
		auto error_uri_val = yyjson_obj_get(obj, "error_uri");
		if (error_uri_val) {
			string error_uri;
			if (yyjson_is_str(error_uri_val)) {
				error_uri = yyjson_get_str(error_uri_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("OAuthError property 'error_uri' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(error_uri_val)));
			}
			builder.SetErrorUri(std::move(error_uri));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

OAuthError OAuthError::FromJSON(yyjson_val *obj) {
	OAuthErrorBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

OAuthError OAuthError::Copy() const {
	return OAuthError(*this);
}

string OAuthError::Validate() const {
	string error;
	if (!StringUtil::CIEquals(_error, "invalid_request") && !StringUtil::CIEquals(_error, "invalid_client") &&
	    !StringUtil::CIEquals(_error, "invalid_grant") && !StringUtil::CIEquals(_error, "unauthorized_client") &&
	    !StringUtil::CIEquals(_error, "unsupported_grant_type") && !StringUtil::CIEquals(_error, "invalid_scope")) {
		return StringUtil::Format("OAuthError property 'error' must be one of [invalid_request, invalid_client, "
		                          "invalid_grant, unauthorized_client, unsupported_grant_type, invalid_scope], not %s",
		                          _error);
	}
	return "";
}

void OAuthError::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: error
	yyjson_mut_obj_add_strcpy(doc, obj, "error", _error.c_str());

	// Serialize: error_description
	if (error_description.has_value()) {
		auto &error_description_value = *error_description;
		yyjson_mut_obj_add_strcpy(doc, obj, "error_description", error_description_value.c_str());
	}

	// Serialize: error_uri
	if (error_uri.has_value()) {
		auto &error_uri_value = *error_uri;
		yyjson_mut_obj_add_strcpy(doc, obj, "error_uri", error_uri_value.c_str());
	}
}

yyjson_mut_val *OAuthError::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
