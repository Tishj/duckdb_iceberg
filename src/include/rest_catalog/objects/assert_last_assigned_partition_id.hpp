
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

class AssertLastAssignedPartitionIdBuilder;

class AssertLastAssignedPartitionId {
public:
	AssertLastAssignedPartitionId(const AssertLastAssignedPartitionId &);
	AssertLastAssignedPartitionId &operator=(const AssertLastAssignedPartitionId &) = delete;
	AssertLastAssignedPartitionId(AssertLastAssignedPartitionId &&);
	AssertLastAssignedPartitionId &operator=(AssertLastAssignedPartitionId &&) = delete;

private:
	friend class AssertLastAssignedPartitionIdBuilder;
	AssertLastAssignedPartitionId(TableRequirementType type_p, int32_t last_assigned_partition_id_p);

public:
	// Deserialization
	static AssertLastAssignedPartitionId FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertLastAssignedPartitionIdBuilder &builder);
	string Validate() const;

	// Copy
	AssertLastAssignedPartitionId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
	const int32_t last_assigned_partition_id;
};

class AssertLastAssignedPartitionIdBuilder {
public:
	AssertLastAssignedPartitionIdBuilder();
	AssertLastAssignedPartitionIdBuilder &SetType(TableRequirementType value);
	AssertLastAssignedPartitionIdBuilder &SetLastAssignedPartitionId(int32_t value);
	string TryBuild(optional<AssertLastAssignedPartitionId> &result);
	AssertLastAssignedPartitionId Build();

private:
	optional<TableRequirementType> type_;
	optional<int32_t> last_assigned_partition_id_;
	bool has_type_ = false;
	bool has_last_assigned_partition_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
