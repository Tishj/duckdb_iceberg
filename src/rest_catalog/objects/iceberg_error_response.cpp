
#include "rest_catalog/objects/iceberg_error_response.hpp"

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

IcebergErrorResponse::IcebergErrorResponse(ErrorModel _error_p) : _error(std::move(_error_p)) {
}
IcebergErrorResponse::IcebergErrorResponse(const IcebergErrorResponse &other) : _error(other._error.Copy()) {
}
IcebergErrorResponse::IcebergErrorResponse(IcebergErrorResponse &&other)
    : IcebergErrorResponse(static_cast<const IcebergErrorResponse &>(other)) {
}

IcebergErrorResponseBuilder::IcebergErrorResponseBuilder() {
}

IcebergErrorResponseBuilder &IcebergErrorResponseBuilder::SetError(ErrorModel value) {
	_error_.emplace(std::move(value));
	has__error_ = true;
	return *this;
}

IcebergErrorResponse IcebergErrorResponseBuilder::Build() {
	if (!has__error_) {
		throw InvalidInputException("IcebergErrorResponse required property 'error' is missing");
	}
	auto result = IcebergErrorResponse(std::move(*_error_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> IcebergErrorResponseBuilder::TryBuild(optional<IcebergErrorResponse> &result) {
	if (!has__error_) {
		return "IcebergErrorResponse required property 'error' is missing";
	}
	auto built = IcebergErrorResponse(std::move(*_error_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> IcebergErrorResponse::TryFromJSON(yyjson_val *obj, IcebergErrorResponseBuilder &builder) {
	try {
		auto _error_val = yyjson_obj_get(obj, "error");
		if (!_error_val) {
			throw InvalidInputException("IcebergErrorResponse required property 'error' is missing");
		} else {
			builder.SetError(ErrorModel::FromJSON(_error_val));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

IcebergErrorResponse IcebergErrorResponse::FromJSON(yyjson_val *obj) {
	IcebergErrorResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

IcebergErrorResponse IcebergErrorResponse::Copy() const {
	return IcebergErrorResponse(*this);
}

optional<string> IcebergErrorResponse::Validate() const {
	optional<string> error;
	error = _error.Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
