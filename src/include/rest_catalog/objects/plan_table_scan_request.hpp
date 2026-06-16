
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/field_name.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class Expression;

class PlanTableScanRequestBuilder;

class PlanTableScanRequest {
public:
	PlanTableScanRequest(const PlanTableScanRequest &) = delete;
	PlanTableScanRequest &operator=(const PlanTableScanRequest &) = delete;
	PlanTableScanRequest(PlanTableScanRequest &&) = default;
	PlanTableScanRequest &operator=(PlanTableScanRequest &&) = delete;

private:
	friend class PlanTableScanRequestBuilder;
	PlanTableScanRequest(optional<int64_t> snapshot_id_p, optional<vector<FieldName>> select_p,
	                     unique_ptr<Expression> filter_p, optional<int64_t> min_rows_requested_p,
	                     optional<bool> case_sensitive_p, optional<bool> use_snapshot_schema_p,
	                     optional<int64_t> start_snapshot_id_p, optional<int64_t> end_snapshot_id_p,
	                     optional<vector<FieldName>> stats_fields_p);

public:
	// Deserialization
	static PlanTableScanRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, PlanTableScanRequestBuilder &builder);
	string Validate() const;

	// Copy
	PlanTableScanRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<int64_t> snapshot_id;
	optional<vector<FieldName>> select;
	unique_ptr<Expression> filter;
	optional<int64_t> min_rows_requested;
	optional<bool> case_sensitive;
	optional<bool> use_snapshot_schema;
	optional<int64_t> start_snapshot_id;
	optional<int64_t> end_snapshot_id;
	optional<vector<FieldName>> stats_fields;
};

class PlanTableScanRequestBuilder {
public:
	PlanTableScanRequestBuilder();
	PlanTableScanRequestBuilder &SetSnapshotId(int64_t value);
	PlanTableScanRequestBuilder &SetSelect(vector<FieldName> value);
	PlanTableScanRequestBuilder &SetFilter(unique_ptr<Expression> value);
	PlanTableScanRequestBuilder &SetMinRowsRequested(int64_t value);
	PlanTableScanRequestBuilder &SetCaseSensitive(bool value);
	PlanTableScanRequestBuilder &SetUseSnapshotSchema(bool value);
	PlanTableScanRequestBuilder &SetStartSnapshotId(int64_t value);
	PlanTableScanRequestBuilder &SetEndSnapshotId(int64_t value);
	PlanTableScanRequestBuilder &SetStatsFields(vector<FieldName> value);
	string TryBuild(optional<PlanTableScanRequest> &result);
	PlanTableScanRequest Build();

private:
	optional<int64_t> snapshot_id_;
	optional<vector<FieldName>> select_;
	unique_ptr<Expression> filter_;
	optional<int64_t> min_rows_requested_;
	optional<bool> case_sensitive_;
	optional<bool> use_snapshot_schema_;
	optional<int64_t> start_snapshot_id_;
	optional<int64_t> end_snapshot_id_;
	optional<vector<FieldName>> stats_fields_;
};

} // namespace rest_api_objects
} // namespace duckdb
