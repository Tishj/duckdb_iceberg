
#include "rest_catalog/objects/fetch_scan_tasks_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

FetchScanTasksResult::FetchScanTasksResult() : scan_tasks(GeneratedObjectAccess::Create<ScanTasks>()) {
}

FetchScanTasksResultBuilder::FetchScanTasksResultBuilder() {
}

FetchScanTasksResultBuilder &FetchScanTasksResultBuilder::SetScanTasks(ScanTasks value) {
	result_.scan_tasks = std::move(value);
	return *this;
}

string FetchScanTasksResultBuilder::TryBuild(FetchScanTasksResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

FetchScanTasksResult FetchScanTasksResultBuilder::Build() {
	FetchScanTasksResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

FetchScanTasksResult FetchScanTasksResult::FromJSON(yyjson_val *obj) {
	FetchScanTasksResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

FetchScanTasksResult FetchScanTasksResult::Copy() const {
	FetchScanTasksResult res;
	res.scan_tasks = scan_tasks.Copy();
	return res;
}

string FetchScanTasksResult::Validate() const {
	string error;
	error = scan_tasks.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string FetchScanTasksResult::TryFromJSON(yyjson_val *obj) {
	string error;
	error = scan_tasks.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	return Validate();
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
