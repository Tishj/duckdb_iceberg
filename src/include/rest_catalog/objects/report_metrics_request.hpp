
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/commit_report.hpp"
#include "rest_catalog/objects/scan_report.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ReportMetricsRequest {
public:
	ReportMetricsRequest();
	ReportMetricsRequest(const ReportMetricsRequest &) = delete;
	ReportMetricsRequest &operator=(const ReportMetricsRequest &) = delete;
	ReportMetricsRequest(ReportMetricsRequest &&) = default;
	ReportMetricsRequest &operator=(ReportMetricsRequest &&) = default;

public:
	// Deserialization
	static ReportMetricsRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(ReportMetricsRequest &result);
	ReportMetricsRequest Build();

private:
	ReportMetricsRequest result_;
	bool has_report_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
