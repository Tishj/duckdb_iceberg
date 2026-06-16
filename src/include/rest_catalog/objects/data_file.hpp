
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/content_file.hpp"
#include "rest_catalog/objects/count_map.hpp"
#include "rest_catalog/objects/value_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class DataFileBuilder;

class DataFile {
public:
	DataFile(const DataFile &) = delete;
	DataFile &operator=(const DataFile &) = delete;
	DataFile(DataFile &&) = default;
	DataFile &operator=(DataFile &&) = delete;

private:
	friend class DataFileBuilder;
	DataFile(ContentFile content_file_p, optional<int64_t> first_row_id_p, optional<CountMap> column_sizes_p,
	         optional<CountMap> value_counts_p, optional<CountMap> null_value_counts_p,
	         optional<CountMap> nan_value_counts_p, optional<ValueMap> lower_bounds_p,
	         optional<ValueMap> upper_bounds_p);

public:
	// Deserialization
	static DataFile FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, DataFileBuilder &builder);
	string Validate() const;

	// Copy
	DataFile Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ContentFile content_file;
	optional<int64_t> first_row_id;
	optional<CountMap> column_sizes;
	optional<CountMap> value_counts;
	optional<CountMap> null_value_counts;
	optional<CountMap> nan_value_counts;
	optional<ValueMap> lower_bounds;
	optional<ValueMap> upper_bounds;
};

class DataFileBuilder {
public:
	DataFileBuilder();
	DataFileBuilder &SetContentFile(ContentFile value);
	DataFileBuilder &SetFirstRowId(int64_t value);
	DataFileBuilder &SetColumnSizes(CountMap value);
	DataFileBuilder &SetValueCounts(CountMap value);
	DataFileBuilder &SetNullValueCounts(CountMap value);
	DataFileBuilder &SetNanValueCounts(CountMap value);
	DataFileBuilder &SetLowerBounds(ValueMap value);
	DataFileBuilder &SetUpperBounds(ValueMap value);
	string TryBuild(optional<DataFile> &result);
	DataFile Build();

private:
	optional<ContentFile> content_file_;
	optional<int64_t> first_row_id_;
	optional<CountMap> column_sizes_;
	optional<CountMap> value_counts_;
	optional<CountMap> null_value_counts_;
	optional<CountMap> nan_value_counts_;
	optional<ValueMap> lower_bounds_;
	optional<ValueMap> upper_bounds_;
};

} // namespace rest_api_objects
} // namespace duckdb
