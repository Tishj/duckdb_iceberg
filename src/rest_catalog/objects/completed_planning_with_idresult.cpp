
#include "rest_catalog/objects/completed_planning_with_idresult.hpp"

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

CompletedPlanningWithIDResult::CompletedPlanningWithIDResult(CompletedPlanningResult completed_planning_result_p,
                                                             Object6 object_6_p)
    : completed_planning_result(std::move(completed_planning_result_p)), object_6(std::move(object_6_p)) {
}
CompletedPlanningWithIDResult::CompletedPlanningWithIDResult(const CompletedPlanningWithIDResult &other)
    : completed_planning_result(other.completed_planning_result.Copy()), object_6(other.object_6.Copy()) {
}
CompletedPlanningWithIDResult::CompletedPlanningWithIDResult(CompletedPlanningWithIDResult &&other)
    : CompletedPlanningWithIDResult(static_cast<const CompletedPlanningWithIDResult &>(other)) {
}
CompletedPlanningWithIDResult::Object6::Object6(string plan_id_p) : plan_id(std::move(plan_id_p)) {
}
CompletedPlanningWithIDResult::Object6::Object6(const CompletedPlanningWithIDResult::Object6 &other)
    : plan_id(other.plan_id) {
}
CompletedPlanningWithIDResult::Object6::Object6(CompletedPlanningWithIDResult::Object6 &&other)
    : Object6(static_cast<const CompletedPlanningWithIDResult::Object6 &>(other)) {
}

CompletedPlanningWithIDResult::Object6Builder::Object6Builder() {
}

CompletedPlanningWithIDResult::Object6Builder &CompletedPlanningWithIDResult::Object6Builder::SetPlanId(string value) {
	plan_id_.emplace(std::move(value));
	has_plan_id_ = true;
	return *this;
}

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6Builder::Build() {
	if (!has_plan_id_) {
		throw InvalidInputException("Object6 required property 'plan-id' is missing");
	}
	auto result = CompletedPlanningWithIDResult::Object6(std::move(*plan_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string
CompletedPlanningWithIDResult::Object6Builder::TryBuild(optional<CompletedPlanningWithIDResult::Object6> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CompletedPlanningWithIDResult::Object6::TryFromJSON(yyjson_val *obj, Object6Builder &builder) {
	try {
		auto plan_id_val = yyjson_obj_get(obj, "plan-id");
		if (!plan_id_val) {
			throw InvalidInputException("Object6 required property 'plan-id' is missing");
		} else {
			string plan_id;
			if (yyjson_is_str(plan_id_val)) {
				plan_id = yyjson_get_str(plan_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Object6 property 'plan_id' is not of type 'string', found '%s' instead",
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

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6::FromJSON(yyjson_val *obj) {
	Object6Builder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CompletedPlanningWithIDResult::Object6 CompletedPlanningWithIDResult::Object6::Copy() const {
	return CompletedPlanningWithIDResult::Object6(*this);
}

string CompletedPlanningWithIDResult::Object6::Validate() const {
	string error;
	return "";
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
	completed_planning_result_.emplace(std::move(value));
	return *this;
}

CompletedPlanningWithIDResultBuilder &
CompletedPlanningWithIDResultBuilder::SetObject6(CompletedPlanningWithIDResult::Object6 value) {
	object_6_.emplace(std::move(value));
	return *this;
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResultBuilder::Build() {
	auto result = CompletedPlanningWithIDResult(std::move(*completed_planning_result_), std::move(*object_6_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CompletedPlanningWithIDResultBuilder::TryBuild(optional<CompletedPlanningWithIDResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CompletedPlanningWithIDResult::TryFromJSON(yyjson_val *obj, CompletedPlanningWithIDResultBuilder &builder) {
	try {
		builder.SetCompletedPlanningResult(CompletedPlanningResult::FromJSON(obj));
		builder.SetObject6(Object6::FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResult::FromJSON(yyjson_val *obj) {
	CompletedPlanningWithIDResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CompletedPlanningWithIDResult CompletedPlanningWithIDResult::Copy() const {
	return CompletedPlanningWithIDResult(*this);
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
