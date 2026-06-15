
#include "rest_catalog/objects/fetch_planning_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

FetchPlanningResult::FetchPlanningResult()
    : completed_planning_result(GeneratedObjectAccess::Create<optional<CompletedPlanningResult>>()),
      failed_planning_result(GeneratedObjectAccess::Create<optional<FailedPlanningResult>>()),
      empty_planning_result(GeneratedObjectAccess::Create<optional<EmptyPlanningResult>>()) {
}

FetchPlanningResultBuilder::FetchPlanningResultBuilder() {
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetCompletedPlanningResult(CompletedPlanningResult value) {
	result_.completed_planning_result = std::move(value);
	return *this;
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetFailedPlanningResult(FailedPlanningResult value) {
	result_.failed_planning_result = std::move(value);
	return *this;
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetEmptyPlanningResult(EmptyPlanningResult value) {
	result_.empty_planning_result = std::move(value);
	return *this;
}

string FetchPlanningResultBuilder::TryBuild(FetchPlanningResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

FetchPlanningResult FetchPlanningResultBuilder::Build() {
	FetchPlanningResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

FetchPlanningResult FetchPlanningResult::FromJSON(yyjson_val *obj) {
	FetchPlanningResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

FetchPlanningResult FetchPlanningResult::Copy() const {
	FetchPlanningResult res;
	if (completed_planning_result.has_value()) {
		res.completed_planning_result = GeneratedObjectAccess::Create<CompletedPlanningResult>();
		(*res.completed_planning_result) = (*completed_planning_result).Copy();
	}
	if (failed_planning_result.has_value()) {
		res.failed_planning_result = GeneratedObjectAccess::Create<FailedPlanningResult>();
		(*res.failed_planning_result) = (*failed_planning_result).Copy();
	}
	if (empty_planning_result.has_value()) {
		res.empty_planning_result = GeneratedObjectAccess::Create<EmptyPlanningResult>();
		(*res.empty_planning_result) = (*empty_planning_result).Copy();
	}
	return res;
}

string FetchPlanningResult::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (completed_planning_result.has_value()) {
		matched_one_of_variants++;
		error = completed_planning_result->Validate();
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
	if (empty_planning_result.has_value()) {
		matched_one_of_variants++;
		error = empty_planning_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "FetchPlanningResult must have exactly one oneOf variant set";
	}
	return "";
}

string FetchPlanningResult::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		completed_planning_result = GeneratedObjectAccess::Create<CompletedPlanningResult>();
		error = completed_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			completed_planning_result = nullopt;
		}
		failed_planning_result = GeneratedObjectAccess::Create<FailedPlanningResult>();
		error = failed_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			failed_planning_result = nullopt;
		}
		empty_planning_result = GeneratedObjectAccess::Create<EmptyPlanningResult>();
		error = empty_planning_result->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			empty_planning_result = nullopt;
		}
		return "FetchPlanningResult failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
}

void FetchPlanningResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (completed_planning_result.has_value()) {
		completed_planning_result->PopulateJSON(doc, obj);
	} else if (failed_planning_result.has_value()) {
		failed_planning_result->PopulateJSON(doc, obj);
	} else if (empty_planning_result.has_value()) {
		empty_planning_result->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *FetchPlanningResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
