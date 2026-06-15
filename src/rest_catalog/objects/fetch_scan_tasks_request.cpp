
#include "rest_catalog/objects/fetch_scan_tasks_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

FetchScanTasksRequest::FetchScanTasksRequest() {
}

FetchScanTasksRequestBuilder::FetchScanTasksRequestBuilder() {
}

FetchScanTasksRequestBuilder &FetchScanTasksRequestBuilder::SetPlanTask(PlanTask value) {
	result_.plan_task = std::move(value);
	has_plan_task_ = true;
	return *this;
}

string FetchScanTasksRequestBuilder::TryBuild(FetchScanTasksRequest &result) {
	if (!has_plan_task_) {
		return "FetchScanTasksRequest required property 'plan-task' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

FetchScanTasksRequest FetchScanTasksRequestBuilder::Build() {
	FetchScanTasksRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

FetchScanTasksRequest FetchScanTasksRequest::FromJSON(yyjson_val *obj) {
	FetchScanTasksRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

FetchScanTasksRequest FetchScanTasksRequest::Copy() const {
	FetchScanTasksRequest res;
	res.plan_task = plan_task.Copy();
	return res;
}

string FetchScanTasksRequest::Validate() const {
	string error;
	error = plan_task.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string FetchScanTasksRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	auto plan_task_val = yyjson_obj_get(obj, "plan-task");
	if (!plan_task_val) {
		return "FetchScanTasksRequest required property 'plan-task' is missing";
	} else {
		error = plan_task.TryFromJSON(plan_task_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void FetchScanTasksRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: plan-task
	yyjson_mut_val *plan_task_val = plan_task.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "plan-task", plan_task_val);
}

yyjson_mut_val *FetchScanTasksRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
