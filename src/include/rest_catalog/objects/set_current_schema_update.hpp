
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetCurrentSchemaUpdate {
public:
	SetCurrentSchemaUpdate();
	SetCurrentSchemaUpdate(const SetCurrentSchemaUpdate &) = delete;
	SetCurrentSchemaUpdate &operator=(const SetCurrentSchemaUpdate &) = delete;
	SetCurrentSchemaUpdate(SetCurrentSchemaUpdate &&) = default;
	SetCurrentSchemaUpdate &operator=(SetCurrentSchemaUpdate &&) = default;

public:
	// Deserialization
	static SetCurrentSchemaUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetCurrentSchemaUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int32_t schema_id;
};

class SetCurrentSchemaUpdateBuilder {
public:
	SetCurrentSchemaUpdateBuilder();
	SetCurrentSchemaUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetCurrentSchemaUpdateBuilder &SetSchemaId(int32_t value);
	string TryBuild(SetCurrentSchemaUpdate &result);
	SetCurrentSchemaUpdate Build();

private:
	SetCurrentSchemaUpdate result_;
	bool has_schema_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
