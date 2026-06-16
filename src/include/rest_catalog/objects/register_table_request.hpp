
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

class RegisterTableRequestBuilder;

class RegisterTableRequest {
public:
	RegisterTableRequest(const RegisterTableRequest &);
	RegisterTableRequest &operator=(const RegisterTableRequest &) = delete;
	RegisterTableRequest(RegisterTableRequest &&);
	RegisterTableRequest &operator=(RegisterTableRequest &&) = delete;

private:
	friend class RegisterTableRequestBuilder;
	RegisterTableRequest(string name_p, string metadata_location_p, optional<bool> overwrite_p);

public:
	// Deserialization
	static RegisterTableRequest FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, RegisterTableRequestBuilder &builder);
	optional<string> Validate() const;

	// Copy
	RegisterTableRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string name;
	const string metadata_location;
	const optional<bool> overwrite;
};

class RegisterTableRequestBuilder {
public:
	RegisterTableRequestBuilder();
	RegisterTableRequestBuilder &SetName(string value);
	RegisterTableRequestBuilder &SetMetadataLocation(string value);
	RegisterTableRequestBuilder &SetOverwrite(bool value);
	optional<string> TryBuild(optional<RegisterTableRequest> &result);
	RegisterTableRequest Build();

private:
	optional<string> name_;
	optional<string> metadata_location_;
	optional<bool> overwrite_;
	bool has_name_ = false;
	bool has_metadata_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
