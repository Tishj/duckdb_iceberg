
#include "rest_catalog/objects/empty_planning_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

EmptyPlanningResult::EmptyPlanningResult() {
}

EmptyPlanningResultBuilder::EmptyPlanningResultBuilder() {
}

EmptyPlanningResultBuilder &EmptyPlanningResultBuilder::SetStatus(PlanStatus value) {
	result_.status = std::move(value);
	has_status_ = true;
	return *this;
}

string EmptyPlanningResultBuilder::TryBuild(EmptyPlanningResult &result) {
	if (!has_status_) {
		return "EmptyPlanningResult required property 'status' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

EmptyPlanningResult EmptyPlanningResultBuilder::Build() {
	EmptyPlanningResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

EmptyPlanningResult EmptyPlanningResult::FromJSON(yyjson_val *obj) {
	EmptyPlanningResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

EmptyPlanningResult EmptyPlanningResult::Copy() const {
	EmptyPlanningResult res;
	res.status = status.Copy();
	return res;
}

string EmptyPlanningResult::Validate() const {
	string error;
	error = status.Validate();
	if (!error.empty()) {
		return error;
	}
	if (status.value != "submitted" && status.value != "cancelled") {
		return "EmptyPlanningResult property 'status' must be one of [submitted, cancelled]";
	}
	return "";
}

string EmptyPlanningResult::TryFromJSON(yyjson_val *obj) {
	string error;
	auto status_val = yyjson_obj_get(obj, "status");
	if (!status_val) {
		return "EmptyPlanningResult required property 'status' is missing";
	} else {
		error = status.TryFromJSON(status_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void EmptyPlanningResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: status
	yyjson_mut_val *status_val = status.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "status", status_val);
}

yyjson_mut_val *EmptyPlanningResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
