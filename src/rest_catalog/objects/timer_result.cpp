
#include "rest_catalog/objects/timer_result.hpp"

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

TimerResult::TimerResult(string time_unit_p, int64_t count_p, int64_t total_duration_p)
    : time_unit(std::move(time_unit_p)), count(std::move(count_p)), total_duration(std::move(total_duration_p)) {
}
TimerResult::TimerResult(const TimerResult &other)
    : time_unit(other.time_unit), count(other.count), total_duration(other.total_duration) {
}
TimerResult::TimerResult(TimerResult &&other) : TimerResult(static_cast<const TimerResult &>(other)) {
}

TimerResultBuilder::TimerResultBuilder() {
}

TimerResultBuilder &TimerResultBuilder::SetTimeUnit(string value) {
	time_unit_.emplace(std::move(value));
	has_time_unit_ = true;
	return *this;
}

TimerResultBuilder &TimerResultBuilder::SetCount(int64_t value) {
	count_.emplace(std::move(value));
	has_count_ = true;
	return *this;
}

TimerResultBuilder &TimerResultBuilder::SetTotalDuration(int64_t value) {
	total_duration_.emplace(std::move(value));
	has_total_duration_ = true;
	return *this;
}

TimerResult TimerResultBuilder::Build() {
	if (!has_time_unit_) {
		throw InvalidInputException("TimerResult required property 'time-unit' is missing");
	}
	if (!has_count_) {
		throw InvalidInputException("TimerResult required property 'count' is missing");
	}
	if (!has_total_duration_) {
		throw InvalidInputException("TimerResult required property 'total-duration' is missing");
	}
	auto result = TimerResult(std::move(*time_unit_), std::move(*count_), std::move(*total_duration_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> TimerResultBuilder::TryBuild(optional<TimerResult> &result) {
	if (!has_time_unit_) {
		return "TimerResult required property 'time-unit' is missing";
	}
	if (!has_count_) {
		return "TimerResult required property 'count' is missing";
	}
	if (!has_total_duration_) {
		return "TimerResult required property 'total-duration' is missing";
	}
	auto built = TimerResult(std::move(*time_unit_), std::move(*count_), std::move(*total_duration_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> TimerResult::TryFromJSON(yyjson_val *obj, TimerResultBuilder &builder) {
	try {
		auto time_unit_val = yyjson_obj_get(obj, "time-unit");
		if (!time_unit_val) {
			throw InvalidInputException("TimerResult required property 'time-unit' is missing");
		} else {
			string time_unit;
			if (yyjson_is_str(time_unit_val)) {
				time_unit = yyjson_get_str(time_unit_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TimerResult property 'time_unit' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(time_unit_val)));
			}
			builder.SetTimeUnit(std::move(time_unit));
		}
		auto count_val = yyjson_obj_get(obj, "count");
		if (!count_val) {
			throw InvalidInputException("TimerResult required property 'count' is missing");
		} else {
			int64_t count;
			if (yyjson_is_sint(count_val)) {
				count = yyjson_get_sint(count_val);
			} else if (yyjson_is_uint(count_val)) {
				count = yyjson_get_uint(count_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TimerResult property 'count' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(count_val)));
			}
			builder.SetCount(std::move(count));
		}
		auto total_duration_val = yyjson_obj_get(obj, "total-duration");
		if (!total_duration_val) {
			throw InvalidInputException("TimerResult required property 'total-duration' is missing");
		} else {
			int64_t total_duration;
			if (yyjson_is_sint(total_duration_val)) {
				total_duration = yyjson_get_sint(total_duration_val);
			} else if (yyjson_is_uint(total_duration_val)) {
				total_duration = yyjson_get_uint(total_duration_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TimerResult property 'total_duration' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(total_duration_val)));
			}
			builder.SetTotalDuration(std::move(total_duration));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TimerResult TimerResult::FromJSON(yyjson_val *obj) {
	TimerResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

TimerResult TimerResult::Copy() const {
	return TimerResult(*this);
}

optional<string> TimerResult::Validate() const {
	optional<string> error;
	return nullopt;
}

void TimerResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: time-unit
	yyjson_mut_obj_add_strcpy(doc, obj, "time-unit", time_unit.c_str());

	// Serialize: count
	yyjson_mut_obj_add_sint(doc, obj, "count", count);

	// Serialize: total-duration
	yyjson_mut_obj_add_sint(doc, obj, "total-duration", total_duration);
}

yyjson_mut_val *TimerResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
