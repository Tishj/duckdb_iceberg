
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/partition_spec.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/sort_order.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CreateTableRequestBuilder;

class CreateTableRequest {
public:
	CreateTableRequest(const CreateTableRequest &) = delete;
	CreateTableRequest &operator=(const CreateTableRequest &) = delete;
	CreateTableRequest(CreateTableRequest &&) = default;
	CreateTableRequest &operator=(CreateTableRequest &&) = default;

private:
	friend class CreateTableRequestBuilder;
	friend class GeneratedObjectAccess;
	CreateTableRequest();

public:
	// Deserialization
	static CreateTableRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CreateTableRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string name;
	Schema schema;
	optional<string> location;
	optional<PartitionSpec> partition_spec;
	optional<SortOrder> write_order;
	optional<bool> stage_create;
	optional<case_insensitive_map_t<string>> properties;
};

class CreateTableRequestBuilder {
public:
	CreateTableRequestBuilder();
	CreateTableRequestBuilder &SetName(string value);
	CreateTableRequestBuilder &SetSchema(Schema value);
	CreateTableRequestBuilder &SetLocation(string value);
	CreateTableRequestBuilder &SetPartitionSpec(PartitionSpec value);
	CreateTableRequestBuilder &SetWriteOrder(SortOrder value);
	CreateTableRequestBuilder &SetStageCreate(bool value);
	CreateTableRequestBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(CreateTableRequest &result);
	CreateTableRequest Build();

private:
	CreateTableRequest result_;
	bool has_name_ = false;
	bool has_schema_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
