
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

class SnapshotLog {
public:
	SnapshotLog(const SnapshotLog &);
	SnapshotLog &operator=(const SnapshotLog &) = delete;
	SnapshotLog(SnapshotLog &&);
	SnapshotLog &operator=(SnapshotLog &&) = delete;
	class Object3Builder;

	class Object3 {
	public:
		Object3(const Object3 &);
		Object3 &operator=(const Object3 &) = delete;
		Object3(Object3 &&);
		Object3 &operator=(Object3 &&) = delete;

	private:
		friend class Object3Builder;
		Object3(int64_t snapshot_id_p, int64_t timestamp_ms_p);

	public:
		// Deserialization
		static Object3 FromJSON(yyjson_val *obj);
		static optional<string> TryFromJSON(yyjson_val *obj, Object3Builder &builder);
		optional<string> Validate() const;

		// Copy
		Object3 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		const int64_t snapshot_id;
		const int64_t timestamp_ms;
	};

	class Object3Builder {
	public:
		Object3Builder();
		Object3Builder &SetSnapshotId(int64_t value);
		Object3Builder &SetTimestampMs(int64_t value);
		optional<string> TryBuild(optional<Object3> &result);
		Object3 Build();

	private:
		optional<int64_t> snapshot_id_;
		optional<int64_t> timestamp_ms_;
		bool has_snapshot_id_ = false;
		bool has_timestamp_ms_ = false;
	};

	SnapshotLog(vector<Object3> value_p);

public:
	// Deserialization
	static SnapshotLog FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, optional<SnapshotLog> &result);
	optional<string> Validate() const;

	// Copy
	SnapshotLog Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const vector<Object3> value;
};

} // namespace rest_api_objects
} // namespace duckdb
