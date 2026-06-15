
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RemoveSchemasUpdateBuilder;

class RemoveSchemasUpdate {
public:
	RemoveSchemasUpdate(const RemoveSchemasUpdate &) = delete;
	RemoveSchemasUpdate &operator=(const RemoveSchemasUpdate &) = delete;
	RemoveSchemasUpdate(RemoveSchemasUpdate &&) = default;
	RemoveSchemasUpdate &operator=(RemoveSchemasUpdate &&) = default;

private:
	friend class RemoveSchemasUpdateBuilder;
	friend class GeneratedObjectAccess;
	RemoveSchemasUpdate();

public:
	// Deserialization
	static RemoveSchemasUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	RemoveSchemasUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	vector<int32_t> schema_ids;
};

class RemoveSchemasUpdateBuilder {
public:
	RemoveSchemasUpdateBuilder();
	RemoveSchemasUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemoveSchemasUpdateBuilder &SetSchemaIds(vector<int32_t> value);
	string TryBuild(RemoveSchemasUpdate &result);
	RemoveSchemasUpdate Build();

private:
	RemoveSchemasUpdate result_;
	bool has_schema_ids_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
