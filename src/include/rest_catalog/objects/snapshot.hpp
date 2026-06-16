
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

class SnapshotBuilder;

class Snapshot {
public:
	Snapshot(const Snapshot &) = delete;
	Snapshot &operator=(const Snapshot &) = delete;
	Snapshot(Snapshot &&) = default;
	Snapshot &operator=(Snapshot &&) = delete;
	class Object2Builder;

	class Object2 {
	public:
		Object2(const Object2 &) = delete;
		Object2 &operator=(const Object2 &) = delete;
		Object2(Object2 &&) = default;
		Object2 &operator=(Object2 &&) = delete;

	private:
		friend class Object2Builder;
		Object2(string operation_p, case_insensitive_map_t<string> additional_properties_p);

	public:
		// Deserialization
		static Object2 FromJSON(yyjson_val *obj);
		static string TryFromJSON(yyjson_val *obj, Object2Builder &builder);
		string Validate() const;

		// Copy
		Object2 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		string operation;
		case_insensitive_map_t<string> additional_properties;
	};

	class Object2Builder {
	public:
		Object2Builder();
		Object2Builder &SetOperation(string value);
		Object2Builder &SetAdditionalProperties(case_insensitive_map_t<string> value);
		string TryBuild(optional<Object2> &result);
		Object2 Build();

	private:
		optional<string> operation_;
		optional<case_insensitive_map_t<string>> additional_properties_;
		bool has_operation_ = false;
	};

private:
	friend class SnapshotBuilder;
	Snapshot(int64_t snapshot_id_p, int64_t timestamp_ms_p, string manifest_list_p, Object2 summary_p,
	         optional<int64_t> parent_snapshot_id_p, optional<int64_t> sequence_number_p,
	         optional<int64_t> first_row_id_p, optional<int64_t> added_rows_p, optional<int32_t> schema_id_p);

public:
	// Deserialization
	static Snapshot FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SnapshotBuilder &builder);
	string Validate() const;

	// Copy
	Snapshot Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int64_t snapshot_id;
	int64_t timestamp_ms;
	string manifest_list;
	Object2 summary;
	optional<int64_t> parent_snapshot_id;
	optional<int64_t> sequence_number;
	optional<int64_t> first_row_id;
	optional<int64_t> added_rows;
	optional<int32_t> schema_id;
};

class SnapshotBuilder {
public:
	SnapshotBuilder();
	SnapshotBuilder &SetSnapshotId(int64_t value);
	SnapshotBuilder &SetTimestampMs(int64_t value);
	SnapshotBuilder &SetManifestList(string value);
	SnapshotBuilder &SetSummary(Snapshot::Object2 value);
	SnapshotBuilder &SetParentSnapshotId(int64_t value);
	SnapshotBuilder &SetSequenceNumber(int64_t value);
	SnapshotBuilder &SetFirstRowId(int64_t value);
	SnapshotBuilder &SetAddedRows(int64_t value);
	SnapshotBuilder &SetSchemaId(int32_t value);
	string TryBuild(optional<Snapshot> &result);
	Snapshot Build();

private:
	optional<int64_t> snapshot_id_;
	optional<int64_t> timestamp_ms_;
	optional<string> manifest_list_;
	optional<Snapshot::Object2> summary_;
	optional<int64_t> parent_snapshot_id_;
	optional<int64_t> sequence_number_;
	optional<int64_t> first_row_id_;
	optional<int64_t> added_rows_;
	optional<int32_t> schema_id_;
	bool has_snapshot_id_ = false;
	bool has_timestamp_ms_ = false;
	bool has_manifest_list_ = false;
	bool has_summary_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
