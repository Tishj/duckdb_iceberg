
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/error_model.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class IcebergErrorResponseBuilder;

class IcebergErrorResponse {
public:
	IcebergErrorResponse(const IcebergErrorResponse &);
	IcebergErrorResponse &operator=(const IcebergErrorResponse &) = delete;
	IcebergErrorResponse(IcebergErrorResponse &&);
	IcebergErrorResponse &operator=(IcebergErrorResponse &&) = delete;

private:
	friend class IcebergErrorResponseBuilder;
	IcebergErrorResponse(ErrorModel _error_p);

public:
	// Deserialization
	static IcebergErrorResponse FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, IcebergErrorResponseBuilder &builder);
	string Validate() const;

	// Copy
	IcebergErrorResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const ErrorModel _error;
};

class IcebergErrorResponseBuilder {
public:
	IcebergErrorResponseBuilder();
	IcebergErrorResponseBuilder &SetError(ErrorModel value);
	string TryBuild(optional<IcebergErrorResponse> &result);
	IcebergErrorResponse Build();

private:
	optional<ErrorModel> _error_;
	bool has__error_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
