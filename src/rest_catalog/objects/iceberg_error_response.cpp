
#include "rest_catalog/objects/iceberg_error_response.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

IcebergErrorResponse::IcebergErrorResponse() : _error(GeneratedObjectAccess::Create<ErrorModel>()) {
}

IcebergErrorResponseBuilder::IcebergErrorResponseBuilder() {
}

IcebergErrorResponseBuilder &IcebergErrorResponseBuilder::SetError(ErrorModel value) {
	result_._error = std::move(value);
	has__error_ = true;
	return *this;
}

string IcebergErrorResponseBuilder::TryBuild(IcebergErrorResponse &result) {
	if (!has__error_) {
		return "IcebergErrorResponse required property 'error' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

IcebergErrorResponse IcebergErrorResponseBuilder::Build() {
	IcebergErrorResponse result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

IcebergErrorResponse IcebergErrorResponse::FromJSON(yyjson_val *obj) {
	IcebergErrorResponse res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

IcebergErrorResponse IcebergErrorResponse::Copy() const {
	IcebergErrorResponse res;
	res._error = _error.Copy();
	return res;
}

string IcebergErrorResponse::Validate() const {
	string error;
	error = _error.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string IcebergErrorResponse::TryFromJSON(yyjson_val *obj) {
	string error;
	auto _error_val = yyjson_obj_get(obj, "error");
	if (!_error_val) {
		return "IcebergErrorResponse required property 'error' is missing";
	} else {
		error = _error.TryFromJSON(_error_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void IcebergErrorResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: error
	yyjson_mut_val *_error_val = _error.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "error", _error_val);
}

yyjson_mut_val *IcebergErrorResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
