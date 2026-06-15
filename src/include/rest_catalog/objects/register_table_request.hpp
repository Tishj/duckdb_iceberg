
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RegisterTableRequest {
public:
	RegisterTableRequest();
	RegisterTableRequest(const RegisterTableRequest &) = delete;
	RegisterTableRequest &operator=(const RegisterTableRequest &) = delete;
	RegisterTableRequest(RegisterTableRequest &&) = default;
	RegisterTableRequest &operator=(RegisterTableRequest &&) = default;

public:
	// Deserialization
	static RegisterTableRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RegisterTableRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string name;
	string metadata_location;
	optional<bool> overwrite;
};

class RegisterTableRequestBuilder {
public:
	RegisterTableRequestBuilder();
	RegisterTableRequestBuilder &SetName(string value);
	RegisterTableRequestBuilder &SetMetadataLocation(string value);
	RegisterTableRequestBuilder &SetOverwrite(bool value);
	string TryBuild(RegisterTableRequest &result);
	RegisterTableRequest Build();

private:
	RegisterTableRequest result_;
	bool has_name_ = false;
	bool has_metadata_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
