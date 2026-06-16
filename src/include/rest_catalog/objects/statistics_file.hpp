
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/blob_metadata.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class StatisticsFileBuilder;

class StatisticsFile {
public:
	StatisticsFile(const StatisticsFile &);
	StatisticsFile &operator=(const StatisticsFile &) = delete;
	StatisticsFile(StatisticsFile &&);
	StatisticsFile &operator=(StatisticsFile &&) = delete;

private:
	friend class StatisticsFileBuilder;
	StatisticsFile(int64_t snapshot_id_p, string statistics_path_p, int64_t file_size_in_bytes_p,
	               int64_t file_footer_size_in_bytes_p, vector<BlobMetadata> blob_metadata_p);

public:
	// Deserialization
	static StatisticsFile FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, StatisticsFileBuilder &builder);
	string Validate() const;

	// Copy
	StatisticsFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int64_t snapshot_id;
	const string statistics_path;
	const int64_t file_size_in_bytes;
	const int64_t file_footer_size_in_bytes;
	const vector<BlobMetadata> blob_metadata;
};

class StatisticsFileBuilder {
public:
	StatisticsFileBuilder();
	StatisticsFileBuilder &SetSnapshotId(int64_t value);
	StatisticsFileBuilder &SetStatisticsPath(string value);
	StatisticsFileBuilder &SetFileSizeInBytes(int64_t value);
	StatisticsFileBuilder &SetFileFooterSizeInBytes(int64_t value);
	StatisticsFileBuilder &SetBlobMetadata(vector<BlobMetadata> value);
	string TryBuild(optional<StatisticsFile> &result);
	StatisticsFile Build();

private:
	optional<int64_t> snapshot_id_;
	optional<string> statistics_path_;
	optional<int64_t> file_size_in_bytes_;
	optional<int64_t> file_footer_size_in_bytes_;
	optional<vector<BlobMetadata>> blob_metadata_;
	bool has_snapshot_id_ = false;
	bool has_statistics_path_ = false;
	bool has_file_size_in_bytes_ = false;
	bool has_file_footer_size_in_bytes_ = false;
	bool has_blob_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
