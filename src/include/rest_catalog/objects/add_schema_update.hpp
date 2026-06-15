
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/schema.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddSchemaUpdateBuilder;

class AddSchemaUpdate {
public:
	AddSchemaUpdate(const AddSchemaUpdate &) = delete;
	AddSchemaUpdate &operator=(const AddSchemaUpdate &) = delete;
	AddSchemaUpdate(AddSchemaUpdate &&) = default;
	AddSchemaUpdate &operator=(AddSchemaUpdate &&) = default;

private:
	friend class AddSchemaUpdateBuilder;
	friend class GeneratedObjectAccess;
	AddSchemaUpdate();

public:
	// Deserialization
	static AddSchemaUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	AddSchemaUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	Schema schema;
	optional<int32_t> last_column_id;
};

class AddSchemaUpdateBuilder {
public:
	AddSchemaUpdateBuilder();
	AddSchemaUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddSchemaUpdateBuilder &SetSchema(Schema value);
	AddSchemaUpdateBuilder &SetLastColumnId(int32_t value);
	string TryBuild(AddSchemaUpdate &result);
	AddSchemaUpdate Build();

private:
	AddSchemaUpdate result_;
	bool has_schema_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
