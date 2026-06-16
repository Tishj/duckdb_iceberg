
#include "rest_catalog/objects/counter_result.hpp"

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

CounterResult::CounterResult(string unit_p, int64_t value_p) : unit(std::move(unit_p)), value(std::move(value_p)) {
}
CounterResult::CounterResult(const CounterResult &other) : unit(other.unit), value(other.value) {
}
CounterResult::CounterResult(CounterResult &&other) : CounterResult(static_cast<const CounterResult &>(other)) {
}

CounterResultBuilder::CounterResultBuilder() {
}

CounterResultBuilder &CounterResultBuilder::SetUnit(string value) {
	unit_.emplace(std::move(value));
	has_unit_ = true;
	return *this;
}

CounterResultBuilder &CounterResultBuilder::SetValue(int64_t value) {
	value_.emplace(std::move(value));
	has_value_ = true;
	return *this;
}

CounterResult CounterResultBuilder::Build() {
	if (!has_unit_) {
		throw InvalidInputException("CounterResult required property 'unit' is missing");
	}
	if (!has_value_) {
		throw InvalidInputException("CounterResult required property 'value' is missing");
	}
	auto result = CounterResult(std::move(*unit_), std::move(*value_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CounterResultBuilder::TryBuild(optional<CounterResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CounterResult::TryFromJSON(yyjson_val *obj, CounterResultBuilder &builder) {
	try {
		auto unit_val = yyjson_obj_get(obj, "unit");
		if (!unit_val) {
			throw InvalidInputException("CounterResult required property 'unit' is missing");
		} else {
			string unit;
			if (yyjson_is_str(unit_val)) {
				unit = yyjson_get_str(unit_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("CounterResult property 'unit' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(unit_val)));
			}
			builder.SetUnit(std::move(unit));
		}
		auto value_val = yyjson_obj_get(obj, "value");
		if (!value_val) {
			throw InvalidInputException("CounterResult required property 'value' is missing");
		} else {
			int64_t value;
			if (yyjson_is_sint(value_val)) {
				value = yyjson_get_sint(value_val);
			} else if (yyjson_is_uint(value_val)) {
				value = yyjson_get_uint(value_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("CounterResult property 'value' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(value_val)));
			}
			builder.SetValue(std::move(value));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CounterResult CounterResult::FromJSON(yyjson_val *obj) {
	CounterResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CounterResult CounterResult::Copy() const {
	return CounterResult(*this);
}

string CounterResult::Validate() const {
	string error;
	return "";
}

void CounterResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: unit
	yyjson_mut_obj_add_strcpy(doc, obj, "unit", unit.c_str());

	// Serialize: value
	yyjson_mut_obj_add_sint(doc, obj, "value", value);
}

yyjson_mut_val *CounterResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
