
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_requirement_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AssertCurrentSchemaIdBuilder;

class AssertCurrentSchemaId {
public:
	AssertCurrentSchemaId(const AssertCurrentSchemaId &);
	AssertCurrentSchemaId &operator=(const AssertCurrentSchemaId &) = delete;
	AssertCurrentSchemaId(AssertCurrentSchemaId &&);
	AssertCurrentSchemaId &operator=(AssertCurrentSchemaId &&) = delete;

private:
	friend class AssertCurrentSchemaIdBuilder;
	AssertCurrentSchemaId(TableRequirementType type_p, int32_t current_schema_id_p);

public:
	// Deserialization
	static AssertCurrentSchemaId FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertCurrentSchemaIdBuilder &builder);
	string Validate() const;

	// Copy
	AssertCurrentSchemaId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
	const int32_t current_schema_id;
};

class AssertCurrentSchemaIdBuilder {
public:
	AssertCurrentSchemaIdBuilder();
	AssertCurrentSchemaIdBuilder &SetType(TableRequirementType value);
	AssertCurrentSchemaIdBuilder &SetCurrentSchemaId(int32_t value);
	string TryBuild(optional<AssertCurrentSchemaId> &result);
	AssertCurrentSchemaId Build();

private:
	optional<TableRequirementType> type_;
	optional<int32_t> current_schema_id_;
	bool has_type_ = false;
	bool has_current_schema_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
