
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
	RegisterViewRequest(const RegisterViewRequest &);
	RegisterViewRequest &operator=(const RegisterViewRequest &) = delete;
	RegisterViewRequest(RegisterViewRequest &&);
	RegisterViewRequest &operator=(RegisterViewRequest &&) = delete;

private:
	friend class RegisterViewRequestBuilder;
	RegisterViewRequest(string name_p, string metadata_location_p);

public:
	// Deserialization
	static RegisterViewRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, RegisterViewRequestBuilder &builder);
	string Validate() const;

	// Copy
	RegisterViewRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string name;
	const string metadata_location;
};

class RegisterViewRequestBuilder {
public:
	RegisterViewRequestBuilder();
	RegisterViewRequestBuilder &SetName(string value);
	RegisterViewRequestBuilder &SetMetadataLocation(string value);
	string TryBuild(optional<RegisterViewRequest> &result);
	RegisterViewRequest Build();

private:
	optional<string> name_;
	optional<string> metadata_location_;
	bool has_name_ = false;
	bool has_metadata_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
