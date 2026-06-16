
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

class EqualityDeleteFileBuilder;

class EqualityDeleteFile {
public:
	EqualityDeleteFile(const EqualityDeleteFile &);
	EqualityDeleteFile &operator=(const EqualityDeleteFile &) = delete;
	EqualityDeleteFile(EqualityDeleteFile &&);
	EqualityDeleteFile &operator=(EqualityDeleteFile &&) = delete;

private:
	friend class EqualityDeleteFileBuilder;
	EqualityDeleteFile(ContentFile content_file_p, optional<vector<int32_t>> equality_ids_p);

public:
	// Deserialization
	static EqualityDeleteFile FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, EqualityDeleteFileBuilder &builder);
	optional<string> Validate() const;

	// Copy
	EqualityDeleteFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const ContentFile content_file;
	const optional<vector<int32_t>> equality_ids;
};

class EqualityDeleteFileBuilder {
public:
	EqualityDeleteFileBuilder();
	EqualityDeleteFileBuilder &SetContentFile(ContentFile value);
	EqualityDeleteFileBuilder &SetEqualityIds(vector<int32_t> value);
	optional<string> TryBuild(optional<EqualityDeleteFile> &result);
	EqualityDeleteFile Build();

private:
	optional<ContentFile> content_file_;
	optional<vector<int32_t>> equality_ids_;
};

} // namespace rest_api_objects
} // namespace duckdb
