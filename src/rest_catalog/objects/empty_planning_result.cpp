
#include "rest_catalog/objects/empty_planning_result.hpp"

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

EmptyPlanningResult::EmptyPlanningResult(PlanStatus status_p) : status(std::move(status_p)) {
}
EmptyPlanningResult::EmptyPlanningResult(const EmptyPlanningResult &other) : status(other.status.Copy()) {
}
EmptyPlanningResult::EmptyPlanningResult(EmptyPlanningResult &&other)
    : EmptyPlanningResult(static_cast<const EmptyPlanningResult &>(other)) {
}

EmptyPlanningResultBuilder::EmptyPlanningResultBuilder() {
}

EmptyPlanningResultBuilder &EmptyPlanningResultBuilder::SetStatus(PlanStatus value) {
	status_.emplace(std::move(value));
	has_status_ = true;
	return *this;
}

EmptyPlanningResult EmptyPlanningResultBuilder::Build() {
	if (!has_status_) {
		throw InvalidInputException("EmptyPlanningResult required property 'status' is missing");
	}
	auto result = EmptyPlanningResult(std::move(*status_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string EmptyPlanningResultBuilder::TryBuild(optional<EmptyPlanningResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string EmptyPlanningResult::TryFromJSON(yyjson_val *obj, EmptyPlanningResultBuilder &builder) {
	try {
		auto status_val = yyjson_obj_get(obj, "status");
		if (!status_val) {
			throw InvalidInputException("EmptyPlanningResult required property 'status' is missing");
		} else {
			builder.SetStatus(PlanStatus::FromJSON(status_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

EmptyPlanningResult EmptyPlanningResult::FromJSON(yyjson_val *obj) {
	EmptyPlanningResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

EmptyPlanningResult EmptyPlanningResult::Copy() const {
	return EmptyPlanningResult(*this);
}

string EmptyPlanningResult::Validate() const {
	string error;
	error = status.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(status.value, "submitted") && !StringUtil::CIEquals(status.value, "cancelled")) {
		return StringUtil::Format("EmptyPlanningResult property 'status' must be one of [submitted, cancelled], not %s",
		                          status.value);
	}
	return "";
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
