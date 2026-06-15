
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

FailedPlanningResult::FailedPlanningResult(IcebergErrorResponse iceberg_error_response_p, Object7 object_7_p)
    : iceberg_error_response(std::move(iceberg_error_response_p)), object_7(std::move(object_7_p)) {
}
FailedPlanningResult::Object7::Object7(PlanStatus status_p) : status(std::move(status_p)) {
}

FailedPlanningResult::Object7Builder::Object7Builder() {
}

FailedPlanningResult::Object7Builder &FailedPlanningResult::Object7Builder::SetStatus(PlanStatus value) {
	status_ = std::move(value);
	has_status_ = true;
	return *this;
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7Builder::Build() {
	if (!has_status_) {
		throw InvalidInputException("Object7 required property 'status' is missing");
	}
	auto result = FailedPlanningResult::Object7(std::move(*status_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FailedPlanningResult::Object7Builder::TryBuild(optional<FailedPlanningResult::Object7> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7::FromJSON(yyjson_val *obj) {
	Object7Builder builder;
	auto status_val = yyjson_obj_get(obj, "status");
	if (!status_val) {
		throw InvalidInputException("Object7 required property 'status' is missing");
	} else {
		optional<PlanStatus> status;
		status = PlanStatus::FromJSON(status_val);
		builder.SetStatus(std::move(*status));
	}
	return builder.Build();
}

string FailedPlanningResult::Object7::TryFromJSON(yyjson_val *obj, optional<FailedPlanningResult::Object7> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FailedPlanningResult::Object7 FailedPlanningResult::Object7::Copy() const {
	Object7Builder builder;
	optional<PlanStatus> status_tmp;
	status_tmp = status.Copy();
	builder.SetStatus(std::move(*status_tmp));
	return builder.Build();
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
	iceberg_error_response_ = std::move(value);
	return *this;
}

FailedPlanningResultBuilder &FailedPlanningResultBuilder::SetObject7(FailedPlanningResult::Object7 value) {
	object_7_ = std::move(value);
	return *this;
}

FailedPlanningResult FailedPlanningResultBuilder::Build() {
	auto result = FailedPlanningResult(std::move(*iceberg_error_response_), std::move(*object_7_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FailedPlanningResultBuilder::TryBuild(optional<FailedPlanningResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FailedPlanningResult FailedPlanningResult::FromJSON(yyjson_val *obj) {
	FailedPlanningResultBuilder builder;
	builder.SetIcebergErrorResponse(IcebergErrorResponse::FromJSON(obj));
	builder.SetObject7(Object7::FromJSON(obj));
	return builder.Build();
}

string FailedPlanningResult::TryFromJSON(yyjson_val *obj, optional<FailedPlanningResult> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FailedPlanningResult FailedPlanningResult::Copy() const {
	FailedPlanningResultBuilder builder;
	optional<IcebergErrorResponse> iceberg_error_response_tmp;
	iceberg_error_response_tmp = iceberg_error_response.Copy();
	builder.SetIcebergErrorResponse(std::move(*iceberg_error_response_tmp));
	optional<Object7> object_7_tmp;
	object_7_tmp = object_7.Copy();
	builder.SetObject7(std::move(*object_7_tmp));
	return builder.Build();
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
