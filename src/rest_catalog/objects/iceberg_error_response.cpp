
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

IcebergErrorResponse::IcebergErrorResponse(ErrorModel _error_p) : _error(std::move(_error_p)) {
}

IcebergErrorResponseBuilder::IcebergErrorResponseBuilder() {
}

IcebergErrorResponseBuilder &IcebergErrorResponseBuilder::SetError(ErrorModel value) {
	_error_ = std::move(value);
	has__error_ = true;
	return *this;
}

IcebergErrorResponse IcebergErrorResponseBuilder::Build() {
	if (!has__error_) {
		throw InvalidInputException("IcebergErrorResponse required property 'error' is missing");
	}
	auto result = IcebergErrorResponse(std::move(*_error_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string IcebergErrorResponseBuilder::TryBuild(optional<IcebergErrorResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

IcebergErrorResponse IcebergErrorResponse::FromJSON(yyjson_val *obj) {
	IcebergErrorResponseBuilder builder;
	auto _error_val = yyjson_obj_get(obj, "error");
	if (!_error_val) {
		throw InvalidInputException("IcebergErrorResponse required property 'error' is missing");
	} else {
		optional<ErrorModel> _error;
		_error = ErrorModel::FromJSON(_error_val);
		builder.SetError(std::move(*_error));
	}
	return builder.Build();
}

string IcebergErrorResponse::TryFromJSON(yyjson_val *obj, optional<IcebergErrorResponse> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

IcebergErrorResponse IcebergErrorResponse::Copy() const {
	IcebergErrorResponseBuilder builder;
	optional<ErrorModel> _error_tmp;
	_error_tmp = _error.Copy();
	builder.SetError(std::move(*_error_tmp));
	return builder.Build();
}

string IcebergErrorResponse::Validate() const {
	string error;
	error = _error.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
