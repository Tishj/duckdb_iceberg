
#include "rest_catalog/objects/async_planning_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AsyncPlanningResult::AsyncPlanningResult() {
}

AsyncPlanningResultBuilder::AsyncPlanningResultBuilder() {
}

AsyncPlanningResultBuilder &AsyncPlanningResultBuilder::SetStatus(PlanStatus value) {
	result_.status = std::move(value);
	has_status_ = true;
	return *this;
}

AsyncPlanningResultBuilder &AsyncPlanningResultBuilder::SetPlanId(string value) {
	result_.plan_id = std::move(value);
	has_plan_id_ = true;
	return *this;
}

string AsyncPlanningResultBuilder::TryBuild(AsyncPlanningResult &result) {
	if (!has_status_) {
		return "AsyncPlanningResult required property 'status' is missing";
	}
	if (!has_plan_id_) {
		return "AsyncPlanningResult required property 'plan-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AsyncPlanningResult AsyncPlanningResultBuilder::Build() {
	AsyncPlanningResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AsyncPlanningResult AsyncPlanningResult::FromJSON(yyjson_val *obj) {
	AsyncPlanningResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AsyncPlanningResult AsyncPlanningResult::Copy() const {
	AsyncPlanningResult res;
	res.status = status.Copy();
	res.plan_id = plan_id;
	return res;
}

string AsyncPlanningResult::Validate() const {
	string error;
	error = status.Validate();
	if (!error.empty()) {
		return error;
	}
	if (status.value != "submitted") {
		return "AsyncPlanningResult property 'status' must be submitted";
	}
	return "";
}

string AsyncPlanningResult::TryFromJSON(yyjson_val *obj) {
	string error;
	auto status_val = yyjson_obj_get(obj, "status");
	if (!status_val) {
		return "AsyncPlanningResult required property 'status' is missing";
	} else {
		error = status.TryFromJSON(status_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto plan_id_val = yyjson_obj_get(obj, "plan-id");
	if (!plan_id_val) {
		return "AsyncPlanningResult required property 'plan-id' is missing";
	} else {
		if (yyjson_is_str(plan_id_val)) {
			plan_id = yyjson_get_str(plan_id_val);
		} else {
			return StringUtil::Format(
			    "AsyncPlanningResult property 'plan_id' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(plan_id_val));
		}
	}
	return Validate();
}

void AsyncPlanningResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: status
	yyjson_mut_val *status_val = status.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "status", status_val);

	// Serialize: plan-id
	yyjson_mut_obj_add_strcpy(doc, obj, "plan-id", plan_id.c_str());
}

yyjson_mut_val *AsyncPlanningResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
