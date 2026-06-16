
#include "rest_catalog/objects/fetch_scan_tasks_result.hpp"

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

FetchScanTasksResult::FetchScanTasksResult(ScanTasks scan_tasks_p) : scan_tasks(std::move(scan_tasks_p)) {
}

FetchScanTasksResultBuilder::FetchScanTasksResultBuilder() {
}

FetchScanTasksResultBuilder &FetchScanTasksResultBuilder::SetScanTasks(ScanTasks value) {
	scan_tasks_.emplace(std::move(value));
	return *this;
}

FetchScanTasksResult FetchScanTasksResultBuilder::Build() {
	auto result = FetchScanTasksResult(std::move(*scan_tasks_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FetchScanTasksResultBuilder::TryBuild(optional<FetchScanTasksResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FetchScanTasksResult FetchScanTasksResult::FromJSON(yyjson_val *obj) {
	FetchScanTasksResultBuilder builder;
	builder.SetScanTasks(ScanTasks::FromJSON(obj));
	return builder.Build();
}

string FetchScanTasksResult::TryFromJSON(yyjson_val *obj, optional<FetchScanTasksResult> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FetchScanTasksResult FetchScanTasksResult::Copy() const {
	FetchScanTasksResultBuilder builder;
	auto scan_tasks_tmp = scan_tasks.Copy();
	builder.SetScanTasks(std::move(scan_tasks_tmp));
	return builder.Build();
}

string FetchScanTasksResult::Validate() const {
	string error;
	error = scan_tasks.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void FetchScanTasksResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: ScanTasks
	scan_tasks.PopulateJSON(doc, obj);
}

yyjson_mut_val *FetchScanTasksResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
