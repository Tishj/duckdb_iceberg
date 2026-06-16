
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/data_file.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class Expression;

class FileScanTaskBuilder;

class FileScanTask {
public:
	FileScanTask(const FileScanTask &);
	FileScanTask &operator=(const FileScanTask &) = delete;
	FileScanTask(FileScanTask &&);
	FileScanTask &operator=(FileScanTask &&) = delete;

private:
	friend class FileScanTaskBuilder;
	FileScanTask(DataFile data_file_p, optional<vector<int32_t>> delete_file_references_p,
	             unique_ptr<Expression> residual_filter_p);

public:
	// Deserialization
	static FileScanTask FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, FileScanTaskBuilder &builder);
	optional<string> Validate() const;

	// Copy
	FileScanTask Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const DataFile data_file;
	const optional<vector<int32_t>> delete_file_references;
	const unique_ptr<Expression> residual_filter;
};

class FileScanTaskBuilder {
public:
	FileScanTaskBuilder();
	FileScanTaskBuilder &SetDataFile(DataFile value);
	FileScanTaskBuilder &SetDeleteFileReferences(vector<int32_t> value);
	FileScanTaskBuilder &SetResidualFilter(unique_ptr<Expression> value);
	optional<string> TryBuild(optional<FileScanTask> &result);
	FileScanTask Build();

private:
	optional<DataFile> data_file_;
	optional<vector<int32_t>> delete_file_references_;
	unique_ptr<Expression> residual_filter_;
	bool has_data_file_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
