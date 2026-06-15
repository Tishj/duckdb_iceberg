
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RegisterViewRequestBuilder;

class RegisterViewRequest {
public:
	RegisterViewRequest(const RegisterViewRequest &) = delete;
	RegisterViewRequest &operator=(const RegisterViewRequest &) = delete;
	RegisterViewRequest(RegisterViewRequest &&) = default;
	RegisterViewRequest &operator=(RegisterViewRequest &&) = default;

private:
	friend class RegisterViewRequestBuilder;
	friend class GeneratedObjectAccess;
	RegisterViewRequest();

public:
	// Deserialization
	static RegisterViewRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RegisterViewRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string name;
	string metadata_location;
};

class RegisterViewRequestBuilder {
public:
	RegisterViewRequestBuilder();
	RegisterViewRequestBuilder &SetName(string value);
	RegisterViewRequestBuilder &SetMetadataLocation(string value);
	string TryBuild(RegisterViewRequest &result);
	RegisterViewRequest Build();

private:
	RegisterViewRequest result_;
	bool has_name_ = false;
	bool has_metadata_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
