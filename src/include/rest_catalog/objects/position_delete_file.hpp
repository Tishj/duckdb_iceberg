
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
	PositionDeleteFile &operator=(PositionDeleteFile &&) = default;

private:
	friend class PositionDeleteFileBuilder;
	friend class GeneratedObjectAccess;
	PositionDeleteFile();

public:
	// Deserialization
	static PositionDeleteFile FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(PositionDeleteFile &result);
	PositionDeleteFile Build();

private:
	PositionDeleteFile result_;
};

} // namespace rest_api_objects
} // namespace duckdb
