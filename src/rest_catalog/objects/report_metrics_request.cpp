
#include "rest_catalog/objects/report_metrics_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ReportMetricsRequest::ReportMetricsRequest() {
}

ReportMetricsRequestBuilder::ReportMetricsRequestBuilder() {
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetScanReport(ScanReport value) {
	result_.scan_report = std::move(value);
	return *this;
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetCommitReport(CommitReport value) {
	result_.commit_report = std::move(value);
	return *this;
}

ReportMetricsRequestBuilder &ReportMetricsRequestBuilder::SetReportType(string value) {
	result_.report_type = std::move(value);
	has_report_type_ = true;
	return *this;
}

string ReportMetricsRequestBuilder::TryBuild(ReportMetricsRequest &result) {
	if (!has_report_type_) {
		return "ReportMetricsRequest required property 'report-type' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

ReportMetricsRequest ReportMetricsRequestBuilder::Build() {
	ReportMetricsRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

ReportMetricsRequest ReportMetricsRequest::FromJSON(yyjson_val *obj) {
	ReportMetricsRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

ReportMetricsRequest ReportMetricsRequest::Copy() const {
	ReportMetricsRequest res;
	if (scan_report.has_value()) {
		res.scan_report.emplace();
		(*res.scan_report) = (*scan_report).Copy();
	}
	if (commit_report.has_value()) {
		res.commit_report.emplace();
		(*res.commit_report) = (*commit_report).Copy();
	}
	res.report_type = report_type;
	return res;
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

string ReportMetricsRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	scan_report.emplace();
	error = scan_report->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		scan_report = nullopt;
	}
	commit_report.emplace();
	error = commit_report->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		commit_report = nullopt;
	}
	if (!(commit_report.has_value()) && !(scan_report.has_value())) {
		return "ReportMetricsRequest failed to parse, none of the anyOf candidates matched";
	}
	auto report_type_val = yyjson_obj_get(obj, "report-type");
	if (!report_type_val) {
		return "ReportMetricsRequest required property 'report-type' is missing";
	} else {
		if (yyjson_is_str(report_type_val)) {
			report_type = yyjson_get_str(report_type_val);
		} else {
			return StringUtil::Format(
			    "ReportMetricsRequest property 'report_type' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(report_type_val));
		}
	}
	return Validate();
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
