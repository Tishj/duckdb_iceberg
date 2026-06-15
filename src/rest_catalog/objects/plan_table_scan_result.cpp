
#include "rest_catalog/objects/plan_table_scan_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

PlanTableScanResult::PlanTableScanResult() {
}

PlanTableScanResultBuilder::PlanTableScanResultBuilder() {
}

PlanTableScanResultBuilder &
PlanTableScanResultBuilder::SetCompletedPlanningWithIdresult(CompletedPlanningWithIDResult value) {
	result_.completed_planning_with_idresult = std::move(value);
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetFailedPlanningResult(FailedPlanningResult value) {
	result_.failed_planning_result = std::move(value);
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetAsyncPlanningResult(AsyncPlanningResult value) {
	result_.async_planning_result = std::move(value);
	return *this;
}

PlanTableScanResultBuilder &PlanTableScanResultBuilder::SetEmptyPlanningResult(EmptyPlanningResult value) {
	result_.empty_planning_result = std::move(value);
	return *this;
}

string PlanTableScanResultBuilder::TryBuild(PlanTableScanResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

PlanTableScanResult PlanTableScanResultBuilder::Build() {
	PlanTableScanResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

PlanTableScanResult PlanTableScanResult::FromJSON(yyjson_val *obj) {
	PlanTableScanResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

PlanTableScanResult PlanTableScanResult::Copy() const {
	PlanTableScanResult res;
	if (completed_planning_with_idresult.has_value()) {
		res.completed_planning_with_idresult.emplace();
		(*res.completed_planning_with_idresult) = (*completed_planning_with_idresult).Copy();
	}
	if (failed_planning_result.has_value()) {
		res.failed_planning_result.emplace();
		(*res.failed_planning_result) = (*failed_planning_result).Copy();
	}
	if (async_planning_result.has_value()) {
		res.async_planning_result.emplace();
		(*res.async_planning_result) = (*async_planning_result).Copy();
	}
	if (empty_planning_result.has_value()) {
		res.empty_planning_result.emplace();
		(*res.empty_planning_result) = (*empty_planning_result).Copy();
	}
	return res;
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

string PlanTableScanResult::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		completed_planning_with_idresult.emplace();
		error = completed_planning_with_idresult->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			completed_planning_with_idresult = nullopt;
		}
		failed_planning_result.emplace();
		error = failed_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			failed_planning_result = nullopt;
		}
		async_planning_result.emplace();
		error = async_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			async_planning_result = nullopt;
		}
		empty_planning_result.emplace();
		error = empty_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			empty_planning_result = nullopt;
		}
		return "PlanTableScanResult failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
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
