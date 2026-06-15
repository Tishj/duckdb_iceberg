
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/error_model.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class IcebergErrorResponse {
public:
	IcebergErrorResponse();
	IcebergErrorResponse(const IcebergErrorResponse &) = delete;
	IcebergErrorResponse &operator=(const IcebergErrorResponse &) = delete;
	IcebergErrorResponse(IcebergErrorResponse &&) = default;
	IcebergErrorResponse &operator=(IcebergErrorResponse &&) = default;

public:
	// Deserialization
	static IcebergErrorResponse FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	IcebergErrorResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ErrorModel _error;
};

class IcebergErrorResponseBuilder {
public:
	IcebergErrorResponseBuilder();
	IcebergErrorResponseBuilder &SetError(ErrorModel value);
	string TryBuild(IcebergErrorResponse &result);
	IcebergErrorResponse Build();

private:
	IcebergErrorResponse result_;
	bool has__error_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
