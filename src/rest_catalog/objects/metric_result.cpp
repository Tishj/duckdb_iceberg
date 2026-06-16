
#include "rest_catalog/objects/metric_result.hpp"

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

MetricResult::MetricResult(optional<CounterResult> counter_result_p, optional<TimerResult> timer_result_p)
    : counter_result(std::move(counter_result_p)), timer_result(std::move(timer_result_p)) {
}

MetricResultBuilder::MetricResultBuilder() {
}

MetricResultBuilder &MetricResultBuilder::SetCounterResult(CounterResult value) {
	counter_result_.emplace(std::move(value));
	return *this;
}

MetricResultBuilder &MetricResultBuilder::SetTimerResult(TimerResult value) {
	timer_result_.emplace(std::move(value));
	return *this;
}

MetricResult MetricResultBuilder::Build() {
	auto result = MetricResult(std::move(counter_result_), std::move(timer_result_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string MetricResultBuilder::TryBuild(optional<MetricResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string MetricResult::TryFromJSON(yyjson_val *obj, MetricResultBuilder &builder) {
	try {
		int matched_any_of_variants = 0;
		try {
			builder.SetCounterResult(CounterResult::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		try {
			builder.SetTimerResult(TimerResult::FromJSON(obj));
			matched_any_of_variants++;
		} catch (const Exception &) {
		}
		if (matched_any_of_variants == 0) {
			throw InvalidInputException("MetricResult failed to parse, none of the anyOf candidates matched");
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MetricResult MetricResult::FromJSON(yyjson_val *obj) {
	MetricResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

MetricResult MetricResult::Copy() const {
	MetricResultBuilder builder;
	optional<CounterResult> counter_result_tmp;
	if (counter_result.has_value()) {
		counter_result_tmp.emplace((*counter_result).Copy());
	}
	if (counter_result_tmp.has_value()) {
		builder.SetCounterResult(std::move(*counter_result_tmp));
	}
	optional<TimerResult> timer_result_tmp;
	if (timer_result.has_value()) {
		timer_result_tmp.emplace((*timer_result).Copy());
	}
	if (timer_result_tmp.has_value()) {
		builder.SetTimerResult(std::move(*timer_result_tmp));
	}
	return builder.Build();
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
