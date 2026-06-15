
#include "rest_catalog/objects/completed_planning_with_idresult.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

CompletedPlanningWithIDResult::CompletedPlanningWithIDResult()
    : completed_planning_result(GeneratedObjectAccess::Create<CompletedPlanningResult>()),
      object_6(GeneratedObjectAccess::Create<Object6>()) {
}
CompletedPlanningWithIDResult::Object6::Object6() {
}

CompletedPlanningWithIDResult::Object6Builder::Object6Builder() {
}

CompletedPlanningWithIDResult::Object6Builder &CompletedPlanningWithIDResult::Object6Builder::SetPlanId(string value) {
	result_.plan_id = std::move(value);
	has_plan_id_ = true;
	return *this;
}

string CompletedPlanningWithIDResult::Object6Builder::TryBuild(CompletedPlanningWithIDResult::Object6 &result) {
	if (!has_plan_id_) {
		return "Object6 required property 'plan-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6Builder::Build() {
	CompletedPlanningWithIDResult::Object6 result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6::FromJSON(yyjson_val *obj) {
	Object6 res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6::Copy() const {
	Object6 res;
	res.plan_id = plan_id;
	return res;
}

string CompletedPlanningWithIDResult::Object6::Validate() const {
	string error;
	return "";
}

string CompletedPlanningWithIDResult::Object6::TryFromJSON(yyjson_val *obj) {
	string error;
	auto plan_id_val = yyjson_obj_get(obj, "plan-id");
	if (!plan_id_val) {
		return "Object6 required property 'plan-id' is missing";
	} else {
		if (yyjson_is_str(plan_id_val)) {
			plan_id = yyjson_get_str(plan_id_val);
		} else {
			return StringUtil::Format("Object6 property 'plan_id' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(plan_id_val));
		}
	}
	return Validate();
}

void CompletedPlanningWithIDResult::Object6::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: plan-id
	yyjson_mut_obj_add_strcpy(doc, obj, "plan-id", plan_id.c_str());
}

yyjson_mut_val *CompletedPlanningWithIDResult::Object6::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

CompletedPlanningWithIDResultBuilder::CompletedPlanningWithIDResultBuilder() {
}

CompletedPlanningWithIDResultBuilder &
CompletedPlanningWithIDResultBuilder::SetCompletedPlanningResult(CompletedPlanningResult value) {
	result_.completed_planning_result = std::move(value);
	return *this;
}

CompletedPlanningWithIDResultBuilder &
CompletedPlanningWithIDResultBuilder::SetObject6(CompletedPlanningWithIDResult::Object6 value) {
	result_.object_6 = std::move(value);
	return *this;
}

string CompletedPlanningWithIDResultBuilder::TryBuild(CompletedPlanningWithIDResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResultBuilder::Build() {
	CompletedPlanningWithIDResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResult::FromJSON(yyjson_val *obj) {
	CompletedPlanningWithIDResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResult::Copy() const {
	CompletedPlanningWithIDResult res;
	res.completed_planning_result = completed_planning_result.Copy();
	res.object_6 = object_6.Copy();
	return res;
}

string CompletedPlanningWithIDResult::Validate() const {
	string error;
	error = completed_planning_result.Validate();
	if (!error.empty()) {
		return error;
	}
	error = object_6.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string CompletedPlanningWithIDResult::TryFromJSON(yyjson_val *obj) {
	string error;
	error = completed_planning_result.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	error = object_6.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	return Validate();
}

void CompletedPlanningWithIDResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: CompletedPlanningResult
	completed_planning_result.PopulateJSON(doc, obj);

	// Serialize base class: Object6
	object_6.PopulateJSON(doc, obj);
}

yyjson_mut_val *CompletedPlanningWithIDResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
