
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SnapshotReferenceBuilder;

class SnapshotReference {
public:
	SnapshotReference(const SnapshotReference &) = delete;
	SnapshotReference &operator=(const SnapshotReference &) = delete;
	SnapshotReference(SnapshotReference &&) = default;
	SnapshotReference &operator=(SnapshotReference &&) = delete;

private:
	friend class SnapshotReferenceBuilder;
	SnapshotReference(string type_p, int64_t snapshot_id_p, optional<int64_t> max_ref_age_ms_p,
	                  optional<int64_t> max_snapshot_age_ms_p, optional<int32_t> min_snapshots_to_keep_p);

public:
	// Deserialization
	static SnapshotReference FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<SnapshotReference> &result);
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
	string TryBuild(optional<SnapshotReference> &result);
	SnapshotReference Build();

private:
	optional<string> type_;
	optional<int64_t> snapshot_id_;
	optional<int64_t> max_ref_age_ms_;
	optional<int64_t> max_snapshot_age_ms_;
	optional<int32_t> min_snapshots_to_keep_;
	bool has_type_ = false;
	bool has_snapshot_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
