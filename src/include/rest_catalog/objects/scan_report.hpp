
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
	ScanReport(const ScanReport &);
	ScanReport &operator=(const ScanReport &) = delete;
	ScanReport(ScanReport &&);
	ScanReport &operator=(ScanReport &&) = delete;

private:
	friend class ScanReportBuilder;
	ScanReport(string table_name_p, int64_t snapshot_id_p, unique_ptr<Expression> filter_p, int32_t schema_id_p,
	           vector<int32_t> projected_field_ids_p, vector<string> projected_field_names_p, Metrics metrics_p,
	           optional<case_insensitive_map_t<string>> metadata_p);

public:
	// Deserialization
	static ScanReport FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, ScanReportBuilder &builder);
	optional<string> Validate() const;

	// Copy
	ScanReport Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string table_name;
	const int64_t snapshot_id;
	const unique_ptr<Expression> filter;
	const int32_t schema_id;
	const vector<int32_t> projected_field_ids;
	const vector<string> projected_field_names;
	const Metrics metrics;
	const optional<case_insensitive_map_t<string>> metadata;
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
	optional<string> TryBuild(optional<ScanReport> &result);
	ScanReport Build();

private:
	optional<string> table_name_;
	optional<int64_t> snapshot_id_;
	unique_ptr<Expression> filter_;
	optional<int32_t> schema_id_;
	optional<vector<int32_t>> projected_field_ids_;
	optional<vector<string>> projected_field_names_;
	optional<Metrics> metrics_;
	optional<case_insensitive_map_t<string>> metadata_;
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
