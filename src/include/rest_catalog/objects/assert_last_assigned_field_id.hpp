
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

class AssertLastAssignedFieldIdBuilder;

class AssertLastAssignedFieldId {
public:
	AssertLastAssignedFieldId(const AssertLastAssignedFieldId &);
	AssertLastAssignedFieldId &operator=(const AssertLastAssignedFieldId &) = delete;
	AssertLastAssignedFieldId(AssertLastAssignedFieldId &&);
	AssertLastAssignedFieldId &operator=(AssertLastAssignedFieldId &&) = delete;

private:
	friend class AssertLastAssignedFieldIdBuilder;
	AssertLastAssignedFieldId(TableRequirementType type_p, int32_t last_assigned_field_id_p);

public:
	// Deserialization
	static AssertLastAssignedFieldId FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, AssertLastAssignedFieldIdBuilder &builder);
	optional<string> Validate() const;

	// Copy
	AssertLastAssignedFieldId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
	const int32_t last_assigned_field_id;
};

class AssertLastAssignedFieldIdBuilder {
public:
	AssertLastAssignedFieldIdBuilder();
	AssertLastAssignedFieldIdBuilder &SetType(TableRequirementType value);
	AssertLastAssignedFieldIdBuilder &SetLastAssignedFieldId(int32_t value);
	optional<string> TryBuild(optional<AssertLastAssignedFieldId> &result);
	AssertLastAssignedFieldId Build();

private:
	optional<TableRequirementType> type_;
	optional<int32_t> last_assigned_field_id_;
	bool has_type_ = false;
	bool has_last_assigned_field_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
