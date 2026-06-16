
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/binary_type_value.hpp"
#include "rest_catalog/objects/file_format.hpp"
#include "rest_catalog/objects/primitive_type_value.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ContentFileBuilder;

class ContentFile {
public:
	ContentFile(const ContentFile &);
	ContentFile &operator=(const ContentFile &) = delete;
	ContentFile(ContentFile &&);
	ContentFile &operator=(ContentFile &&) = delete;

private:
	friend class ContentFileBuilder;
	ContentFile(int32_t spec_id_p, vector<PrimitiveTypeValue> partition_p, string content_p, string file_path_p,
	            FileFormat file_format_p, int64_t file_size_in_bytes_p, int64_t record_count_p,
	            optional<BinaryTypeValue> key_metadata_p, optional<vector<int64_t>> split_offsets_p,
	            optional<int32_t> sort_order_id_p);

public:
	// Deserialization
	static ContentFile FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ContentFileBuilder &builder);
	string Validate() const;

	// Copy
	ContentFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t spec_id;
	const vector<PrimitiveTypeValue> partition;
	const string content;
	const string file_path;
	const FileFormat file_format;
	const int64_t file_size_in_bytes;
	const int64_t record_count;
	const optional<BinaryTypeValue> key_metadata;
	const optional<vector<int64_t>> split_offsets;
	const optional<int32_t> sort_order_id;
};

class ContentFileBuilder {
public:
	ContentFileBuilder();
	ContentFileBuilder &SetSpecId(int32_t value);
	ContentFileBuilder &SetPartition(vector<PrimitiveTypeValue> value);
	ContentFileBuilder &SetContent(string value);
	ContentFileBuilder &SetFilePath(string value);
	ContentFileBuilder &SetFileFormat(FileFormat value);
	ContentFileBuilder &SetFileSizeInBytes(int64_t value);
	ContentFileBuilder &SetRecordCount(int64_t value);
	ContentFileBuilder &SetKeyMetadata(BinaryTypeValue value);
	ContentFileBuilder &SetSplitOffsets(vector<int64_t> value);
	ContentFileBuilder &SetSortOrderId(int32_t value);
	string TryBuild(optional<ContentFile> &result);
	ContentFile Build();

private:
	optional<int32_t> spec_id_;
	optional<vector<PrimitiveTypeValue>> partition_;
	optional<string> content_;
	optional<string> file_path_;
	optional<FileFormat> file_format_;
	optional<int64_t> file_size_in_bytes_;
	optional<int64_t> record_count_;
	optional<BinaryTypeValue> key_metadata_;
	optional<vector<int64_t>> split_offsets_;
	optional<int32_t> sort_order_id_;
	bool has_spec_id_ = false;
	bool has_partition_ = false;
	bool has_content_ = false;
	bool has_file_path_ = false;
	bool has_file_format_ = false;
	bool has_file_size_in_bytes_ = false;
	bool has_record_count_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
