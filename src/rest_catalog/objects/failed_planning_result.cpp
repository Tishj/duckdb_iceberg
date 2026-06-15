
#include "rest_catalog/objects/failed_planning_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

FailedPlanningResult::FailedPlanningResult()
    : iceberg_error_response(GeneratedObjectAccess::Create<IcebergErrorResponse>()),
      object_7(GeneratedObjectAccess::Create<Object7>()) {
}
FailedPlanningResult::Object7::Object7() {
}

FailedPlanningResult::Object7Builder::Object7Builder() {
}

FailedPlanningResult::Object7Builder &FailedPlanningResult::Object7Builder::SetStatus(PlanStatus value) {
	result_.status = std::move(value);
	has_status_ = true;
	return *this;
}

string FailedPlanningResult::Object7Builder::TryBuild(FailedPlanningResult::Object7 &result) {
	if (!has_status_) {
		return "Object7 required property 'status' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7Builder::Build() {
	FailedPlanningResult::Object7 result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7::FromJSON(yyjson_val *obj) {
	Object7 res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7::Copy() const {
	Object7 res;
	res.status = status.Copy();
	return res;
}

string FailedPlanningResult::Object7::Validate() const {
	string error;
	error = status.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(status.value, "failed")) {
		return StringUtil::Format("Object7 property 'status' must be failed, not %s", status.value);
	}
	return "";
}

string FailedPlanningResult::Object7::TryFromJSON(yyjson_val *obj) {
	string error;
	auto status_val = yyjson_obj_get(obj, "status");
	if (!status_val) {
		return "Object7 required property 'status' is missing";
	} else {
		error = status.TryFromJSON(status_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void FailedPlanningResult::Object7::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: status
	yyjson_mut_val *status_val = status.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "status", status_val);
}

yyjson_mut_val *FailedPlanningResult::Object7::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

FailedPlanningResultBuilder::FailedPlanningResultBuilder() {
}

FailedPlanningResultBuilder &FailedPlanningResultBuilder::SetIcebergErrorResponse(IcebergErrorResponse value) {
	result_.iceberg_error_response = std::move(value);
	return *this;
}

FailedPlanningResultBuilder &FailedPlanningResultBuilder::SetObject7(FailedPlanningResult::Object7 value) {
	result_.object_7 = std::move(value);
	return *this;
}

string FailedPlanningResultBuilder::TryBuild(FailedPlanningResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

FailedPlanningResult FailedPlanningResultBuilder::Build() {
	FailedPlanningResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

FailedPlanningResult FailedPlanningResult::FromJSON(yyjson_val *obj) {
	FailedPlanningResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

FailedPlanningResult FailedPlanningResult::Copy() const {
	FailedPlanningResult res;
	res.iceberg_error_response = iceberg_error_response.Copy();
	res.object_7 = object_7.Copy();
	return res;
}

string FailedPlanningResult::Validate() const {
	string error;
	error = iceberg_error_response.Validate();
	if (!error.empty()) {
		return error;
	}
	error = object_7.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string FailedPlanningResult::TryFromJSON(yyjson_val *obj) {
	string error;
	error = iceberg_error_response.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	error = object_7.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	return Validate();
}

void FailedPlanningResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: IcebergErrorResponse
	iceberg_error_response.PopulateJSON(doc, obj);

	// Serialize base class: Object7
	object_7.PopulateJSON(doc, obj);
}

yyjson_mut_val *FailedPlanningResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
