
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
FetchScanTasksRequest::FetchScanTasksRequest(const FetchScanTasksRequest &other) : plan_task(other.plan_task.Copy()) {
}
FetchScanTasksRequest::FetchScanTasksRequest(FetchScanTasksRequest &&other)
    : FetchScanTasksRequest(static_cast<const FetchScanTasksRequest &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> FetchScanTasksRequestBuilder::TryBuild(optional<FetchScanTasksRequest> &result) {
	if (!has_plan_task_) {
		return "FetchScanTasksRequest required property 'plan-task' is missing";
	}
	auto built = FetchScanTasksRequest(std::move(*plan_task_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> FetchScanTasksRequest::TryFromJSON(yyjson_val *obj, FetchScanTasksRequestBuilder &builder) {
	try {
		auto plan_task_val = yyjson_obj_get(obj, "plan-task");
		if (!plan_task_val) {
			throw InvalidInputException("FetchScanTasksRequest required property 'plan-task' is missing");
		} else {
			builder.SetPlanTask(PlanTask::FromJSON(plan_task_val));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FetchScanTasksRequest FetchScanTasksRequest::FromJSON(yyjson_val *obj) {
	FetchScanTasksRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

FetchScanTasksRequest FetchScanTasksRequest::Copy() const {
	return FetchScanTasksRequest(*this);
}

optional<string> FetchScanTasksRequest::Validate() const {
	optional<string> error;
	error = plan_task.Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
