
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

class SetCurrentSchemaUpdateBuilder;

class SetCurrentSchemaUpdate {
public:
	SetCurrentSchemaUpdate(const SetCurrentSchemaUpdate &);
	SetCurrentSchemaUpdate &operator=(const SetCurrentSchemaUpdate &) = delete;
	SetCurrentSchemaUpdate(SetCurrentSchemaUpdate &&);
	SetCurrentSchemaUpdate &operator=(SetCurrentSchemaUpdate &&) = delete;

private:
	friend class SetCurrentSchemaUpdateBuilder;
	SetCurrentSchemaUpdate(BaseUpdate base_update_p, int32_t schema_id_p);

public:
	// Deserialization
	static SetCurrentSchemaUpdate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, SetCurrentSchemaUpdateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	SetCurrentSchemaUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const int32_t schema_id;
};

class SetCurrentSchemaUpdateBuilder {
public:
	SetCurrentSchemaUpdateBuilder();
	SetCurrentSchemaUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetCurrentSchemaUpdateBuilder &SetSchemaId(int32_t value);
	optional<string> TryBuild(optional<SetCurrentSchemaUpdate> &result);
	SetCurrentSchemaUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int32_t> schema_id_;
	bool has_schema_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
