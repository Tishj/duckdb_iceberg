
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/view_version.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CreateViewRequest {
public:
	CreateViewRequest();
	CreateViewRequest(const CreateViewRequest &) = delete;
	CreateViewRequest &operator=(const CreateViewRequest &) = delete;
	CreateViewRequest(CreateViewRequest &&) = default;
	CreateViewRequest &operator=(CreateViewRequest &&) = default;

public:
	// Deserialization
	static CreateViewRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CreateViewRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string name;
	Schema schema;
	ViewVersion view_version;
	case_insensitive_map_t<string> properties;
	optional<string> location;
};

class CreateViewRequestBuilder {
public:
	CreateViewRequestBuilder();
	CreateViewRequestBuilder &SetName(string value);
	CreateViewRequestBuilder &SetSchema(Schema value);
	CreateViewRequestBuilder &SetViewVersion(ViewVersion value);
	CreateViewRequestBuilder &SetProperties(case_insensitive_map_t<string> value);
	CreateViewRequestBuilder &SetLocation(string value);
	string TryBuild(CreateViewRequest &result);
	CreateViewRequest Build();

private:
	CreateViewRequest result_;
	bool has_name_ = false;
	bool has_schema_ = false;
	bool has_view_version_ = false;
	bool has_properties_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
