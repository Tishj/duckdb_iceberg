
#include "rest_catalog/objects/timer_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TimerResult::TimerResult() {
}

TimerResultBuilder::TimerResultBuilder() {
}

TimerResultBuilder &TimerResultBuilder::SetTimeUnit(string value) {
	result_.time_unit = std::move(value);
	has_time_unit_ = true;
	return *this;
}

TimerResultBuilder &TimerResultBuilder::SetCount(int64_t value) {
	result_.count = std::move(value);
	has_count_ = true;
	return *this;
}

TimerResultBuilder &TimerResultBuilder::SetTotalDuration(int64_t value) {
	result_.total_duration = std::move(value);
	has_total_duration_ = true;
	return *this;
}

string TimerResultBuilder::TryBuild(TimerResult &result) {
	if (!has_time_unit_) {
		return "TimerResult required property 'time-unit' is missing";
	}
	if (!has_count_) {
		return "TimerResult required property 'count' is missing";
	}
	if (!has_total_duration_) {
		return "TimerResult required property 'total-duration' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

TimerResult TimerResultBuilder::Build() {
	TimerResult result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

TimerResult TimerResult::FromJSON(yyjson_val *obj) {
	TimerResult res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

TimerResult TimerResult::Copy() const {
	TimerResult res;
	res.time_unit = time_unit;
	res.count = count;
	res.total_duration = total_duration;
	return res;
}

string TimerResult::Validate() const {
	string error;
	return "";
}

string TimerResult::TryFromJSON(yyjson_val *obj) {
	string error;
	auto time_unit_val = yyjson_obj_get(obj, "time-unit");
	if (!time_unit_val) {
		return "TimerResult required property 'time-unit' is missing";
	} else {
		if (yyjson_is_str(time_unit_val)) {
			time_unit = yyjson_get_str(time_unit_val);
		} else {
			return StringUtil::Format("TimerResult property 'time_unit' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(time_unit_val));
		}
	}
	auto count_val = yyjson_obj_get(obj, "count");
	if (!count_val) {
		return "TimerResult required property 'count' is missing";
	} else {
		if (yyjson_is_sint(count_val)) {
			count = yyjson_get_sint(count_val);
		} else if (yyjson_is_uint(count_val)) {
			count = yyjson_get_uint(count_val);
		} else {
			return StringUtil::Format("TimerResult property 'count' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(count_val));
		}
	}
	auto total_duration_val = yyjson_obj_get(obj, "total-duration");
	if (!total_duration_val) {
		return "TimerResult required property 'total-duration' is missing";
	} else {
		if (yyjson_is_sint(total_duration_val)) {
			total_duration = yyjson_get_sint(total_duration_val);
		} else if (yyjson_is_uint(total_duration_val)) {
			total_duration = yyjson_get_uint(total_duration_val);
		} else {
			return StringUtil::Format(
			    "TimerResult property 'total_duration' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(total_duration_val));
		}
	}
	return Validate();
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
