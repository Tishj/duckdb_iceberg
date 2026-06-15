
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/commit_report.hpp"
#include "rest_catalog/objects/scan_report.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ReportMetricsRequestBuilder;

class ReportMetricsRequest {
public:
	ReportMetricsRequest(const ReportMetricsRequest &) = delete;
	ReportMetricsRequest &operator=(const ReportMetricsRequest &) = delete;
	ReportMetricsRequest(ReportMetricsRequest &&) = default;
	ReportMetricsRequest &operator=(ReportMetricsRequest &&) = delete;

private:
	friend class ReportMetricsRequestBuilder;
	ReportMetricsRequest(optional<ScanReport> scan_report_p, optional<CommitReport> commit_report_p,
	                     string report_type_p);

public:
	// Deserialization
	static ReportMetricsRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<ReportMetricsRequest> &result);
	string Validate() const;

	// Copy
	ReportMetricsRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<ScanReport> scan_report;
	optional<CommitReport> commit_report;
	string report_type;
};

class ReportMetricsRequestBuilder {
public:
	ReportMetricsRequestBuilder();
	ReportMetricsRequestBuilder &SetScanReport(ScanReport value);
	ReportMetricsRequestBuilder &SetCommitReport(CommitReport value);
	ReportMetricsRequestBuilder &SetReportType(string value);
	string TryBuild(optional<ReportMetricsRequest> &result);
	ReportMetricsRequest Build();

private:
	optional<ScanReport> scan_report_;
	optional<CommitReport> commit_report_;
	optional<string> report_type_;
	bool has_report_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
