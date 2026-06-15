
#include "rest_catalog/objects/metric_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

MetricResult::MetricResult()
    : counter_result(GeneratedObjectAccess::Create<optional<CounterResult>>()),
      timer_result(GeneratedObjectAccess::Create<optional<TimerResult>>()) {
}

MetricResultBuilder::MetricResultBuilder() {
}

MetricResultBuilder &MetricResultBuilder::SetCounterResult(CounterResult value) {
	result_.counter_result = std::move(value);
	return *this;
}

MetricResultBuilder &MetricResultBuilder::SetTimerResult(TimerResult value) {
	result_.timer_result = std::move(value);
	return *this;
}

string MetricResultBuilder::TryBuild(MetricResult &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

MetricResult MetricResultBuilder::Build() {
	MetricResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

MetricResult MetricResult::FromJSON(yyjson_val *obj) {
	MetricResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

MetricResult MetricResult::Copy() const {
	MetricResult res;
	if (counter_result.has_value()) {
		res.counter_result = GeneratedObjectAccess::Create<CounterResult>();
		(*res.counter_result) = (*counter_result).Copy();
	}
	if (timer_result.has_value()) {
		res.timer_result = GeneratedObjectAccess::Create<TimerResult>();
		(*res.timer_result) = (*timer_result).Copy();
	}
	return res;
}

string MetricResult::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (counter_result.has_value()) {
		matched_any_of_variants++;
		error = counter_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (timer_result.has_value()) {
		matched_any_of_variants++;
		error = timer_result->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "MetricResult must have at least one anyOf variant set";
	}
	return "";
}

string MetricResult::TryFromJSON(yyjson_val *obj) {
	string error;
	counter_result = GeneratedObjectAccess::Create<CounterResult>();
	error = counter_result->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		counter_result = nullopt;
	}
	timer_result = GeneratedObjectAccess::Create<TimerResult>();
	error = timer_result->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		timer_result = nullopt;
	}
	if (!(counter_result.has_value()) && !(timer_result.has_value())) {
		return "MetricResult failed to parse, none of the anyOf candidates matched";
	}
	return Validate();
}

void MetricResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (counter_result.has_value()) {
		counter_result->PopulateJSON(doc, obj);
	} else if (timer_result.has_value()) {
		timer_result->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *MetricResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
