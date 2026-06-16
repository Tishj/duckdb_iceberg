
#include "rest_catalog/objects/fetch_scan_tasks_request.hpp"

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

FetchScanTasksRequest::FetchScanTasksRequest(PlanTask plan_task_p) : plan_task(std::move(plan_task_p)) {
}

FetchScanTasksRequestBuilder::FetchScanTasksRequestBuilder() {
}

FetchScanTasksRequestBuilder &FetchScanTasksRequestBuilder::SetPlanTask(PlanTask value) {
	plan_task_.emplace(std::move(value));
	has_plan_task_ = true;
	return *this;
}

FetchScanTasksRequest FetchScanTasksRequestBuilder::Build() {
	if (!has_plan_task_) {
		throw InvalidInputException("FetchScanTasksRequest required property 'plan-task' is missing");
	}
	auto result = FetchScanTasksRequest(std::move(*plan_task_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FetchScanTasksRequestBuilder::TryBuild(optional<FetchScanTasksRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string FetchScanTasksRequest::TryFromJSON(yyjson_val *obj, FetchScanTasksRequestBuilder &builder) {
	try {
		auto plan_task_val = yyjson_obj_get(obj, "plan-task");
		if (!plan_task_val) {
			throw InvalidInputException("FetchScanTasksRequest required property 'plan-task' is missing");
		} else {
			builder.SetPlanTask(PlanTask::FromJSON(plan_task_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FetchScanTasksRequest FetchScanTasksRequest::FromJSON(yyjson_val *obj) {
	FetchScanTasksRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

FetchScanTasksRequest FetchScanTasksRequest::Copy() const {
	FetchScanTasksRequestBuilder builder;
	auto plan_task_tmp = plan_task.Copy();
	builder.SetPlanTask(std::move(plan_task_tmp));
	return builder.Build();
}

string FetchScanTasksRequest::Validate() const {
	string error;
	error = plan_task.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
