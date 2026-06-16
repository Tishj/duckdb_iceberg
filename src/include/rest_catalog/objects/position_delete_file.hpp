
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/content_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PositionDeleteFileBuilder;

class PositionDeleteFile {
public:
	PositionDeleteFile(const PositionDeleteFile &) = delete;
	PositionDeleteFile &operator=(const PositionDeleteFile &) = delete;
	PositionDeleteFile(PositionDeleteFile &&) = default;
	PositionDeleteFile &operator=(PositionDeleteFile &&) = delete;

private:
	friend class PositionDeleteFileBuilder;
	PositionDeleteFile(ContentFile content_file_p, optional<int64_t> content_offset_p,
	                   optional<int64_t> content_size_in_bytes_p);

public:
	// Deserialization
	static PositionDeleteFile FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, PositionDeleteFileBuilder &builder);
	string Validate() const;

	// Copy
	PositionDeleteFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ContentFile content_file;
	optional<int64_t> content_offset;
	optional<int64_t> content_size_in_bytes;
};

class PositionDeleteFileBuilder {
public:
	PositionDeleteFileBuilder();
	PositionDeleteFileBuilder &SetContentFile(ContentFile value);
	PositionDeleteFileBuilder &SetContentOffset(int64_t value);
	PositionDeleteFileBuilder &SetContentSizeInBytes(int64_t value);
	string TryBuild(optional<PositionDeleteFile> &result);
	PositionDeleteFile Build();

private:
	optional<ContentFile> content_file_;
	optional<int64_t> content_offset_;
	optional<int64_t> content_size_in_bytes_;
};

} // namespace rest_api_objects
} // namespace duckdb
