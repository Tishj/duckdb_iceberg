
#include "rest_catalog/objects/plan_table_scan_result.hpp"

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

PlanTableScanResult::PlanTableScanResult(optional<CompletedPlanningWithIDResult> completed_planning_with_idresult_p,
                                         optional<FailedPlanningResult> failed_planning_result_p,
                                         optional<AsyncPlanningResult> async_planning_result_p,
                                         optional<EmptyPlanningResult> empty_planning_result_p)
    : completed_planning_with_idresult(std::move(completed_planning_with_idresult_p)),
      failed_planning_result(std::move(failed_planning_result_p)),
      async_planning_result(std::move(async_planning_result_p)),
      empty_planning_result(std::move(empty_planning_result_p)) {
}

PlanTableScanResultBuilder::PlanTableScanResultBuilder() {
}

PlanTableScanResultBuilder &
PlanTableScanResultBuilder::SetCompletedPlanningWithIdresult(CompletedPlanningWithIDResult value) {
	completed_planning_with_idresult_.emplace(std::move(value));
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetFailedPlanningResult(FailedPlanningResult value) {
	failed_planning_result_.emplace(std::move(value));
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetAsyncPlanningResult(AsyncPlanningResult value) {
	async_planning_result_.emplace(std::move(value));
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetEmptyPlanningResult(EmptyPlanningResult value) {
	empty_planning_result_.emplace(std::move(value));
	return *this;
}

PlanTableScanResult PlanTableScanResultBuilder::Build() {
	auto result = PlanTableScanResult(std::move(completed_planning_with_idresult_), std::move(failed_planning_result_),
	                                  std::move(async_planning_result_), std::move(empty_planning_result_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PlanTableScanResultBuilder::TryBuild(optional<PlanTableScanResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanTableScanResult PlanTableScanResult::FromJSON(yyjson_val *obj) {
	PlanTableScanResultBuilder builder;
	do {
		try {
			builder.SetCompletedPlanningWithIdresult(CompletedPlanningWithIDResult::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetFailedPlanningResult(FailedPlanningResult::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAsyncPlanningResult(AsyncPlanningResult::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetEmptyPlanningResult(EmptyPlanningResult::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		throw InvalidInputException("PlanTableScanResult failed to parse, none of the oneOf candidates matched");
	} while (false);
	return builder.Build();
}

string PlanTableScanResult::TryFromJSON(yyjson_val *obj, optional<PlanTableScanResult> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanTableScanResult PlanTableScanResult::Copy() const {
	PlanTableScanResultBuilder builder;
	optional<CompletedPlanningWithIDResult> completed_planning_with_idresult_tmp;
	if (completed_planning_with_idresult.has_value()) {
		completed_planning_with_idresult_tmp.emplace((*completed_planning_with_idresult).Copy());
	}
	if (completed_planning_with_idresult_tmp.has_value()) {
		builder.SetCompletedPlanningWithIdresult(std::move(*completed_planning_with_idresult_tmp));
	}
	optional<FailedPlanningResult> failed_planning_result_tmp;
	if (failed_planning_result.has_value()) {
		failed_planning_result_tmp.emplace((*failed_planning_result).Copy());
	}
	if (failed_planning_result_tmp.has_value()) {
		builder.SetFailedPlanningResult(std::move(*failed_planning_result_tmp));
	}
	optional<AsyncPlanningResult> async_planning_result_tmp;
	if (async_planning_result.has_value()) {
		async_planning_result_tmp.emplace((*async_planning_result).Copy());
	}
	if (async_planning_result_tmp.has_value()) {
		builder.SetAsyncPlanningResult(std::move(*async_planning_result_tmp));
	}
	optional<EmptyPlanningResult> empty_planning_result_tmp;
	if (empty_planning_result.has_value()) {
		empty_planning_result_tmp.emplace((*empty_planning_result).Copy());
	}
	if (empty_planning_result_tmp.has_value()) {
		builder.SetEmptyPlanningResult(std::move(*empty_planning_result_tmp));
	}
	return builder.Build();
}

string PlanTableScanResult::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (completed_planning_with_idresult.has_value()) {
		matched_one_of_variants++;
		error = completed_planning_with_idresult->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (failed_planning_result.has_value()) {
		matched_one_of_variants++;
		error = failed_planning_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (async_planning_result.has_value()) {
		matched_one_of_variants++;
		error = async_planning_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (empty_planning_result.has_value()) {
		matched_one_of_variants++;
		error = empty_planning_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "PlanTableScanResult must have exactly one oneOf variant set";
	}
	return "";
}

void PlanTableScanResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (completed_planning_with_idresult.has_value()) {
		completed_planning_with_idresult->PopulateJSON(doc, obj);
	} else if (failed_planning_result.has_value()) {
		failed_planning_result->PopulateJSON(doc, obj);
	} else if (async_planning_result.has_value()) {
		async_planning_result->PopulateJSON(doc, obj);
	} else if (empty_planning_result.has_value()) {
		empty_planning_result->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *PlanTableScanResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
