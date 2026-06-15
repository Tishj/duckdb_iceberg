
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

class PartitionStatisticsFileBuilder;

class PartitionStatisticsFile {
public:
	PartitionStatisticsFile(const PartitionStatisticsFile &) = delete;
	PartitionStatisticsFile &operator=(const PartitionStatisticsFile &) = delete;
	PartitionStatisticsFile(PartitionStatisticsFile &&) = default;
	PartitionStatisticsFile &operator=(PartitionStatisticsFile &&) = default;

private:
	friend class PartitionStatisticsFileBuilder;
	friend class GeneratedObjectAccess;
	PartitionStatisticsFile();

public:
	// Deserialization
	static PartitionStatisticsFile FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	PartitionStatisticsFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	int64_t snapshot_id;
	string statistics_path;
	int64_t file_size_in_bytes;
};

class PartitionStatisticsFileBuilder {
public:
	PartitionStatisticsFileBuilder();
	PartitionStatisticsFileBuilder &SetSnapshotId(int64_t value);
	PartitionStatisticsFileBuilder &SetStatisticsPath(string value);
	PartitionStatisticsFileBuilder &SetFileSizeInBytes(int64_t value);
	string TryBuild(PartitionStatisticsFile &result);
	PartitionStatisticsFile Build();

private:
	PartitionStatisticsFile result_;
	bool has_snapshot_id_ = false;
	bool has_statistics_path_ = false;
	bool has_file_size_in_bytes_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
