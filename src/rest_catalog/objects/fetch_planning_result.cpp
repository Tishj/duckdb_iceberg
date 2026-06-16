
#include "rest_catalog/objects/fetch_planning_result.hpp"

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

FetchPlanningResult::FetchPlanningResult(optional<CompletedPlanningResult> completed_planning_result_p,
                                         optional<FailedPlanningResult> failed_planning_result_p,
                                         optional<EmptyPlanningResult> empty_planning_result_p)
    : completed_planning_result(std::move(completed_planning_result_p)),
      failed_planning_result(std::move(failed_planning_result_p)),
      empty_planning_result(std::move(empty_planning_result_p)) {
}
FetchPlanningResult::FetchPlanningResult(const FetchPlanningResult &other)
    : completed_planning_result((other.completed_planning_result.has_value()
                                     ? optional<CompletedPlanningResult>((*other.completed_planning_result).Copy())
                                     : optional<CompletedPlanningResult>())),
      failed_planning_result((other.failed_planning_result.has_value()
                                  ? optional<FailedPlanningResult>((*other.failed_planning_result).Copy())
                                  : optional<FailedPlanningResult>())),
      empty_planning_result((other.empty_planning_result.has_value()
                                 ? optional<EmptyPlanningResult>((*other.empty_planning_result).Copy())
                                 : optional<EmptyPlanningResult>())) {
}
FetchPlanningResult::FetchPlanningResult(FetchPlanningResult &&other)
    : FetchPlanningResult(static_cast<const FetchPlanningResult &>(other)) {
}

FetchPlanningResultBuilder::FetchPlanningResultBuilder() {
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetCompletedPlanningResult(CompletedPlanningResult value) {
	completed_planning_result_.emplace(std::move(value));
	return *this;
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetFailedPlanningResult(FailedPlanningResult value) {
	failed_planning_result_.emplace(std::move(value));
	return *this;
}

FetchPlanningResultBuilder &FetchPlanningResultBuilder::SetEmptyPlanningResult(EmptyPlanningResult value) {
	empty_planning_result_.emplace(std::move(value));
	return *this;
}

FetchPlanningResult FetchPlanningResultBuilder::Build() {
	auto result = FetchPlanningResult(std::move(completed_planning_result_), std::move(failed_planning_result_),
	                                  std::move(empty_planning_result_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FetchPlanningResultBuilder::TryBuild(optional<FetchPlanningResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string FetchPlanningResult::TryFromJSON(yyjson_val *obj, FetchPlanningResultBuilder &builder) {
	try {
		do {
			try {
				builder.SetCompletedPlanningResult(CompletedPlanningResult::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetFailedPlanningResult(FailedPlanningResult::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetEmptyPlanningResult(EmptyPlanningResult::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			throw InvalidInputException("FetchPlanningResult failed to parse, none of the oneOf candidates matched");
		} while (false);
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FetchPlanningResult FetchPlanningResult::FromJSON(yyjson_val *obj) {
	FetchPlanningResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

FetchPlanningResult FetchPlanningResult::Copy() const {
	return FetchPlanningResult(*this);
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
