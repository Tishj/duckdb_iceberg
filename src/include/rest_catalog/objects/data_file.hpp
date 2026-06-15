
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
	DataFile &operator=(DataFile &&) = default;

private:
	friend class DataFileBuilder;
	friend class GeneratedObjectAccess;
	DataFile();

public:
	// Deserialization
	static DataFile FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(DataFile &result);
	DataFile Build();

private:
	DataFile result_;
};

} // namespace rest_api_objects
} // namespace duckdb
