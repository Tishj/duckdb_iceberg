
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SnapshotReference {
public:
	SnapshotReference();
	SnapshotReference(const SnapshotReference &) = delete;
	SnapshotReference &operator=(const SnapshotReference &) = delete;
	SnapshotReference(SnapshotReference &&) = default;
	SnapshotReference &operator=(SnapshotReference &&) = default;

public:
	// Deserialization
	static SnapshotReference FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SnapshotReference Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	int64_t snapshot_id;
	optional<int64_t> max_ref_age_ms;
	optional<int64_t> max_snapshot_age_ms;
	optional<int32_t> min_snapshots_to_keep;
};

class SnapshotReferenceBuilder {
public:
	SnapshotReferenceBuilder();
	SnapshotReferenceBuilder &SetType(string value);
	SnapshotReferenceBuilder &SetSnapshotId(int64_t value);
	SnapshotReferenceBuilder &SetMaxRefAgeMs(int64_t value);
	SnapshotReferenceBuilder &SetMaxSnapshotAgeMs(int64_t value);
	SnapshotReferenceBuilder &SetMinSnapshotsToKeep(int32_t value);
	string TryBuild(SnapshotReference &result);
	SnapshotReference Build();

private:
	SnapshotReference result_;
	bool has_type_ = false;
	bool has_snapshot_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
