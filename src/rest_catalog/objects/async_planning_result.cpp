
#include "rest_catalog/objects/async_planning_result.hpp"

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

AsyncPlanningResult::AsyncPlanningResult(PlanStatus status_p, string plan_id_p)
    : status(std::move(status_p)), plan_id(std::move(plan_id_p)) {
}

AsyncPlanningResultBuilder::AsyncPlanningResultBuilder() {
}

AsyncPlanningResultBuilder &AsyncPlanningResultBuilder::SetStatus(PlanStatus value) {
	status_.emplace(std::move(value));
	has_status_ = true;
	return *this;
}

AsyncPlanningResultBuilder &AsyncPlanningResultBuilder::SetPlanId(string value) {
	plan_id_.emplace(std::move(value));
	has_plan_id_ = true;
	return *this;
}

AsyncPlanningResult AsyncPlanningResultBuilder::Build() {
	if (!has_status_) {
		throw InvalidInputException("AsyncPlanningResult required property 'status' is missing");
	}
	if (!has_plan_id_) {
		throw InvalidInputException("AsyncPlanningResult required property 'plan-id' is missing");
	}
	auto result = AsyncPlanningResult(std::move(*status_), std::move(*plan_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AsyncPlanningResultBuilder::TryBuild(optional<AsyncPlanningResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AsyncPlanningResult::TryFromJSON(yyjson_val *obj, AsyncPlanningResultBuilder &builder) {
	try {
		auto status_val = yyjson_obj_get(obj, "status");
		if (!status_val) {
			throw InvalidInputException("AsyncPlanningResult required property 'status' is missing");
		} else {
			builder.SetStatus(PlanStatus::FromJSON(status_val));
		}
		auto plan_id_val = yyjson_obj_get(obj, "plan-id");
		if (!plan_id_val) {
			throw InvalidInputException("AsyncPlanningResult required property 'plan-id' is missing");
		} else {
			string plan_id;
			if (yyjson_is_str(plan_id_val)) {
				plan_id = yyjson_get_str(plan_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "AsyncPlanningResult property 'plan_id' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(plan_id_val)));
			}
			builder.SetPlanId(std::move(plan_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AsyncPlanningResult AsyncPlanningResult::FromJSON(yyjson_val *obj) {
	AsyncPlanningResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AsyncPlanningResult AsyncPlanningResult::Copy() const {
	AsyncPlanningResultBuilder builder;
	auto status_tmp = status.Copy();
	builder.SetStatus(std::move(status_tmp));
	string plan_id_tmp;
	plan_id_tmp = plan_id;
	builder.SetPlanId(std::move(plan_id_tmp));
	return builder.Build();
}

string AsyncPlanningResult::Validate() const {
	string error;
	error = status.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(status.value, "submitted")) {
		return StringUtil::Format("AsyncPlanningResult property 'status' must be submitted, not %s", status.value);
	}
	return "";
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
