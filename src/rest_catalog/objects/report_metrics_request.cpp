
#include "rest_catalog/objects/report_metrics_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ReportMetricsRequest::ReportMetricsRequest(optional<ScanReport> scan_report_p, optional<CommitReport> commit_report_p,
                                           string report_type_p)
    : scan_report(std::move(scan_report_p)), commit_report(std::move(commit_report_p)),
      report_type(std::move(report_type_p)) {
}
ReportMetricsRequest::ReportMetricsRequest(const ReportMetricsRequest &other)
    : scan_report(
          (other.scan_report.has_value() ? optional<ScanReport>((*other.scan_report).Copy()) : optional<ScanReport>())),
      commit_report((other.commit_report.has_value() ? optional<CommitReport>((*other.commit_report).Copy())
                                                     : optional<CommitReport>())),
      report_type(other.report_type) {
}
ReportMetricsRequest::ReportMetricsRequest(ReportMetricsRequest &&other)
    : ReportMetricsRequest(static_cast<const ReportMetricsRequest &>(other)) {
}

ReportMetricsRequestBuilder::ReportMetricsRequestBuilder() {
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetScanReport(ScanReport value) {
	scan_report_.emplace(std::move(value));
	return *this;
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetCommitReport(CommitReport value) {
	commit_report_.emplace(std::move(value));
	return *this;
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetReportType(string value) {
	report_type_.emplace(std::move(value));
	has_report_type_ = true;
	return *this;
}

ReportMetricsRequest ReportMetricsRequestBuilder::Build() {
	if (!has_report_type_) {
		throw InvalidInputException("ReportMetricsRequest required property 'report-type' is missing");
	}
	auto result = ReportMetricsRequest(std::move(scan_report_), std::move(commit_report_), std::move(*report_type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ReportMetricsRequestBuilder::TryBuild(optional<ReportMetricsRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string ReportMetricsRequest::TryFromJSON(yyjson_val *obj, ReportMetricsRequestBuilder &builder) {
	try {
		int matched_any_of_variants = 0;
		try {
			builder.SetScanReport(ScanReport::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetCommitReport(CommitReport::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		if (matched_any_of_variants == 0) {
			throw InvalidInputException("ReportMetricsRequest failed to parse, none of the anyOf candidates matched");
		}
		auto report_type_val = yyjson_obj_get(obj, "report-type");
		if (!report_type_val) {
			throw InvalidInputException("ReportMetricsRequest required property 'report-type' is missing");
		} else {
			string report_type;
			if (yyjson_is_str(report_type_val)) {
				report_type = yyjson_get_str(report_type_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ReportMetricsRequest property 'report_type' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(report_type_val)));
			}
			builder.SetReportType(std::move(report_type));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ReportMetricsRequest ReportMetricsRequest::FromJSON(yyjson_val *obj) {
	ReportMetricsRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

ReportMetricsRequest ReportMetricsRequest::Copy() const {
	return ReportMetricsRequest(*this);
}

string ReportMetricsRequest::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (scan_report.has_value()) {
		matched_any_of_variants++;
		error = scan_report->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (commit_report.has_value()) {
		matched_any_of_variants++;
		error = commit_report->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "ReportMetricsRequest must have at least one anyOf variant set";
	}
	return "";
}

void ReportMetricsRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (scan_report.has_value()) {
		scan_report->PopulateJSON(doc, obj);
	} else if (commit_report.has_value()) {
		commit_report->PopulateJSON(doc, obj);
	}

	// Serialize: report-type
	yyjson_mut_obj_add_strcpy(doc, obj, "report-type", report_type.c_str());
}

yyjson_mut_val *ReportMetricsRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
