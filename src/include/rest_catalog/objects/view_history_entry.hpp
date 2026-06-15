
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

class ViewHistoryEntryBuilder;

class ViewHistoryEntry {
public:
	ViewHistoryEntry(const ViewHistoryEntry &) = delete;
	ViewHistoryEntry &operator=(const ViewHistoryEntry &) = delete;
	ViewHistoryEntry(ViewHistoryEntry &&) = default;
	ViewHistoryEntry &operator=(ViewHistoryEntry &&) = delete;

private:
	friend class ViewHistoryEntryBuilder;
	ViewHistoryEntry(int32_t version_id_p, int64_t timestamp_ms_p);

public:
	// Deserialization
	static ViewHistoryEntry FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<ViewHistoryEntry> &result);
	string Validate() const;

	// Copy
	ViewHistoryEntry Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int32_t version_id;
	int64_t timestamp_ms;
};

class ViewHistoryEntryBuilder {
public:
	ViewHistoryEntryBuilder();
	ViewHistoryEntryBuilder &SetVersionId(int32_t value);
	ViewHistoryEntryBuilder &SetTimestampMs(int64_t value);
	string TryBuild(optional<ViewHistoryEntry> &result);
	ViewHistoryEntry Build();

private:
	optional<int32_t> version_id_;
	optional<int64_t> timestamp_ms_;
	bool has_version_id_ = false;
	bool has_timestamp_ms_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
