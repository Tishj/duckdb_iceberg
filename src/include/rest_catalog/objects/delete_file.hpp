
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/equality_delete_file.hpp"
#include "rest_catalog/objects/position_delete_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class DeleteFileBuilder;

class DeleteFile {
public:
	DeleteFile(const DeleteFile &);
	DeleteFile &operator=(const DeleteFile &) = delete;
	DeleteFile(DeleteFile &&);
	DeleteFile &operator=(DeleteFile &&) = delete;

private:
	friend class DeleteFileBuilder;
	DeleteFile(optional<PositionDeleteFile> position_delete_file_p,
	           optional<EqualityDeleteFile> equality_delete_file_p);

public:
	// Deserialization
	static DeleteFile FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, DeleteFileBuilder &builder);
	string Validate() const;

	// Copy
	DeleteFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<PositionDeleteFile> position_delete_file;
	const optional<EqualityDeleteFile> equality_delete_file;
};

class DeleteFileBuilder {
public:
	DeleteFileBuilder();
	DeleteFileBuilder &SetPositionDeleteFile(PositionDeleteFile value);
	DeleteFileBuilder &SetEqualityDeleteFile(EqualityDeleteFile value);
	string TryBuild(optional<DeleteFile> &result);
	DeleteFile Build();

private:
	optional<PositionDeleteFile> position_delete_file_;
	optional<EqualityDeleteFile> equality_delete_file_;
};

} // namespace rest_api_objects
} // namespace duckdb
