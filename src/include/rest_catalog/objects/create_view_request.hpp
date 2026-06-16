
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/view_version.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CreateViewRequestBuilder;

class CreateViewRequest {
public:
	CreateViewRequest(const CreateViewRequest &);
	CreateViewRequest &operator=(const CreateViewRequest &) = delete;
	CreateViewRequest(CreateViewRequest &&);
	CreateViewRequest &operator=(CreateViewRequest &&) = delete;

private:
	friend class CreateViewRequestBuilder;
	CreateViewRequest(string name_p, Schema schema_p, ViewVersion view_version_p,
	                  case_insensitive_map_t<string> properties_p, optional<string> location_p);

public:
	// Deserialization
	static CreateViewRequest FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CreateViewRequestBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CreateViewRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string name;
	const Schema schema;
	const ViewVersion view_version;
	const case_insensitive_map_t<string> properties;
	const optional<string> location;
};

class CreateViewRequestBuilder {
public:
	CreateViewRequestBuilder();
	CreateViewRequestBuilder &SetName(string value);
	CreateViewRequestBuilder &SetSchema(Schema value);
	CreateViewRequestBuilder &SetViewVersion(ViewVersion value);
	CreateViewRequestBuilder &SetProperties(case_insensitive_map_t<string> value);
	CreateViewRequestBuilder &SetLocation(string value);
	optional<string> TryBuild(optional<CreateViewRequest> &result);
	CreateViewRequest Build();

private:
	optional<string> name_;
	optional<Schema> schema_;
	optional<ViewVersion> view_version_;
	optional<case_insensitive_map_t<string>> properties_;
	optional<string> location_;
	bool has_name_ = false;
	bool has_schema_ = false;
	bool has_view_version_ = false;
	bool has_properties_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
