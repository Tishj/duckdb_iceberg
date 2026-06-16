
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
MetricResult::MetricResult(const MetricResult &other)
    : counter_result((other.counter_result.has_value() ? optional<CounterResult>((*other.counter_result).Copy())
                                                       : optional<CounterResult>())),
      timer_result((other.timer_result.has_value() ? optional<TimerResult>((*other.timer_result).Copy())
                                                   : optional<TimerResult>())) {
}
MetricResult::MetricResult(MetricResult &&other) : MetricResult(static_cast<const MetricResult &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> MetricResultBuilder::TryBuild(optional<MetricResult> &result) {
	auto built = MetricResult(std::move(counter_result_), std::move(timer_result_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> MetricResult::TryFromJSON(yyjson_val *obj, MetricResultBuilder &builder) {
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MetricResult MetricResult::FromJSON(yyjson_val *obj) {
	MetricResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

MetricResult MetricResult::Copy() const {
	return MetricResult(*this);
}

optional<string> MetricResult::Validate() const {
	optional<string> error;
	int matched_any_of_variants = 0;
	if (counter_result.has_value()) {
		matched_any_of_variants++;
		error = counter_result->Validate();
		if (error) {
			return error;
		}
	}
	if (timer_result.has_value()) {
		matched_any_of_variants++;
		error = timer_result->Validate();
		if (error) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "MetricResult must have at least one anyOf variant set";
	}
	return nullopt;
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
