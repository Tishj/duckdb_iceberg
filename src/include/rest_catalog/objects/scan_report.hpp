
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/metrics.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class Expression;

class ScanReportBuilder;

class ScanReport {
public:
	ScanReport(const ScanReport &) = delete;
	ScanReport &operator=(const ScanReport &) = delete;
	ScanReport(ScanReport &&) = default;
	ScanReport &operator=(ScanReport &&) = default;

private:
	friend class ScanReportBuilder;
	friend class GeneratedObjectAccess;
	ScanReport();

public:
	// Deserialization
	static ScanReport FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	ScanReport Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string table_name;
	int64_t snapshot_id;
	unique_ptr<Expression> filter;
	int32_t schema_id;
	vector<int32_t> projected_field_ids;
	vector<string> projected_field_names;
	Metrics metrics;
	optional<case_insensitive_map_t<string>> metadata;
};

class ScanReportBuilder {
public:
	ScanReportBuilder();
	ScanReportBuilder &SetTableName(string value);
	ScanReportBuilder &SetSnapshotId(int64_t value);
	ScanReportBuilder &SetFilter(unique_ptr<Expression> value);
	ScanReportBuilder &SetSchemaId(int32_t value);
	ScanReportBuilder &SetProjectedFieldIds(vector<int32_t> value);
	ScanReportBuilder &SetProjectedFieldNames(vector<string> value);
	ScanReportBuilder &SetMetrics(Metrics value);
	ScanReportBuilder &SetMetadata(case_insensitive_map_t<string> value);
	string TryBuild(ScanReport &result);
	ScanReport Build();

private:
	ScanReport result_;
	bool has_table_name_ = false;
	bool has_snapshot_id_ = false;
	bool has_filter_ = false;
	bool has_schema_id_ = false;
	bool has_projected_field_ids_ = false;
	bool has_projected_field_names_ = false;
	bool has_metrics_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
