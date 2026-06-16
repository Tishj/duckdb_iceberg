
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

class AssertRefSnapshotIdBuilder;

class AssertRefSnapshotId {
public:
	AssertRefSnapshotId(const AssertRefSnapshotId &) = delete;
	AssertRefSnapshotId &operator=(const AssertRefSnapshotId &) = delete;
	AssertRefSnapshotId(AssertRefSnapshotId &&) = default;
	AssertRefSnapshotId &operator=(AssertRefSnapshotId &&) = delete;

private:
	friend class AssertRefSnapshotIdBuilder;
	AssertRefSnapshotId(TableRequirementType type_p, string ref_p, optional<int64_t> snapshot_id_p);

public:
	// Deserialization
	static AssertRefSnapshotId FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertRefSnapshotIdBuilder &builder);
	string Validate() const;

	// Copy
	AssertRefSnapshotId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	TableRequirementType type;
	string ref;
	optional<int64_t> snapshot_id;
};

class AssertRefSnapshotIdBuilder {
public:
	AssertRefSnapshotIdBuilder();
	AssertRefSnapshotIdBuilder &SetType(TableRequirementType value);
	AssertRefSnapshotIdBuilder &SetRef(string value);
	AssertRefSnapshotIdBuilder &SetSnapshotId(int64_t value);
	string TryBuild(optional<AssertRefSnapshotId> &result);
	AssertRefSnapshotId Build();

private:
	optional<TableRequirementType> type_;
	optional<string> ref_;
	optional<int64_t> snapshot_id_;
	bool has_type_ = false;
	bool has_ref_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
