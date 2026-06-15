
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
	ViewHistoryEntry &operator=(ViewHistoryEntry &&) = default;

private:
	friend class ViewHistoryEntryBuilder;
	friend class GeneratedObjectAccess;
	ViewHistoryEntry();

public:
	// Deserialization
	static ViewHistoryEntry FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(ViewHistoryEntry &result);
	ViewHistoryEntry Build();

private:
	ViewHistoryEntry result_;
	bool has_version_id_ = false;
	bool has_timestamp_ms_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
