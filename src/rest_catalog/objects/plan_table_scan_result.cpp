
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
PlanTableScanResult::PlanTableScanResult(const PlanTableScanResult &other)
    : completed_planning_with_idresult(
          (other.completed_planning_with_idresult.has_value()
               ? optional<CompletedPlanningWithIDResult>((*other.completed_planning_with_idresult).Copy())
               : optional<CompletedPlanningWithIDResult>())),
      failed_planning_result((other.failed_planning_result.has_value()
                                  ? optional<FailedPlanningResult>((*other.failed_planning_result).Copy())
                                  : optional<FailedPlanningResult>())),
      async_planning_result((other.async_planning_result.has_value()
                                 ? optional<AsyncPlanningResult>((*other.async_planning_result).Copy())
                                 : optional<AsyncPlanningResult>())),
      empty_planning_result((other.empty_planning_result.has_value()
                                 ? optional<EmptyPlanningResult>((*other.empty_planning_result).Copy())
                                 : optional<EmptyPlanningResult>())) {
}
PlanTableScanResult::PlanTableScanResult(PlanTableScanResult &&other)
    : PlanTableScanResult(static_cast<const PlanTableScanResult &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> PlanTableScanResultBuilder::TryBuild(optional<PlanTableScanResult> &result) {
	auto built = PlanTableScanResult(std::move(completed_planning_with_idresult_), std::move(failed_planning_result_),
	                                 std::move(async_planning_result_), std::move(empty_planning_result_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> PlanTableScanResult::TryFromJSON(yyjson_val *obj, PlanTableScanResultBuilder &builder) {
	try {
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanTableScanResult PlanTableScanResult::FromJSON(yyjson_val *obj) {
	PlanTableScanResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

PlanTableScanResult PlanTableScanResult::Copy() const {
	return PlanTableScanResult(*this);
}

optional<string> PlanTableScanResult::Validate() const {
	optional<string> error;
	int matched_one_of_variants = 0;
	if (completed_planning_with_idresult.has_value()) {
		matched_one_of_variants++;
		error = completed_planning_with_idresult->Validate();
		if (error) {
			return error;
		}
	}
	if (failed_planning_result.has_value()) {
		matched_one_of_variants++;
		error = failed_planning_result->Validate();
		if (error) {
			return error;
		}
	}
	if (async_planning_result.has_value()) {
		matched_one_of_variants++;
		error = async_planning_result->Validate();
		if (error) {
			return error;
		}
	}
	if (empty_planning_result.has_value()) {
		matched_one_of_variants++;
		error = empty_planning_result->Validate();
		if (error) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "PlanTableScanResult must have exactly one oneOf variant set";
	}
	return nullopt;
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
