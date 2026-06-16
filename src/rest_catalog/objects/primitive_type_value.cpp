
#include "rest_catalog/objects/primitive_type_value.hpp"

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

PrimitiveTypeValue::PrimitiveTypeValue(
    optional<BooleanTypeValue> boolean_type_value_p, optional<IntegerTypeValue> integer_type_value_p,
    optional<LongTypeValue> long_type_value_p, optional<FloatTypeValue> float_type_value_p,
    optional<DoubleTypeValue> double_type_value_p, optional<DecimalTypeValue> decimal_type_value_p,
    optional<StringTypeValue> string_type_value_p, optional<UUIDTypeValue> uuidtype_value_p,
    optional<DateTypeValue> date_type_value_p, optional<TimeTypeValue> time_type_value_p,
    optional<TimestampTypeValue> timestamp_type_value_p, optional<TimestampTzTypeValue> timestamp_tz_type_value_p,
    optional<TimestampNanoTypeValue> timestamp_nano_type_value_p,
    optional<TimestampTzNanoTypeValue> timestamp_tz_nano_type_value_p, optional<FixedTypeValue> fixed_type_value_p,
    optional<BinaryTypeValue> binary_type_value_p)
    : boolean_type_value(std::move(boolean_type_value_p)), integer_type_value(std::move(integer_type_value_p)),
      long_type_value(std::move(long_type_value_p)), float_type_value(std::move(float_type_value_p)),
      double_type_value(std::move(double_type_value_p)), decimal_type_value(std::move(decimal_type_value_p)),
      string_type_value(std::move(string_type_value_p)), uuidtype_value(std::move(uuidtype_value_p)),
      date_type_value(std::move(date_type_value_p)), time_type_value(std::move(time_type_value_p)),
      timestamp_type_value(std::move(timestamp_type_value_p)),
      timestamp_tz_type_value(std::move(timestamp_tz_type_value_p)),
      timestamp_nano_type_value(std::move(timestamp_nano_type_value_p)),
      timestamp_tz_nano_type_value(std::move(timestamp_tz_nano_type_value_p)),
      fixed_type_value(std::move(fixed_type_value_p)), binary_type_value(std::move(binary_type_value_p)) {
}

PrimitiveTypeValueBuilder::PrimitiveTypeValueBuilder() {
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetBooleanTypeValue(BooleanTypeValue value) {
	boolean_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetIntegerTypeValue(IntegerTypeValue value) {
	integer_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetLongTypeValue(LongTypeValue value) {
	long_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetFloatTypeValue(FloatTypeValue value) {
	float_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDoubleTypeValue(DoubleTypeValue value) {
	double_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDecimalTypeValue(DecimalTypeValue value) {
	decimal_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetStringTypeValue(StringTypeValue value) {
	string_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetUuidtypeValue(UUIDTypeValue value) {
	uuidtype_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDateTypeValue(DateTypeValue value) {
	date_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimeTypeValue(TimeTypeValue value) {
	time_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTypeValue(TimestampTypeValue value) {
	timestamp_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTzTypeValue(TimestampTzTypeValue value) {
	timestamp_tz_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampNanoTypeValue(TimestampNanoTypeValue value) {
	timestamp_nano_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTzNanoTypeValue(TimestampTzNanoTypeValue value) {
	timestamp_tz_nano_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetFixedTypeValue(FixedTypeValue value) {
	fixed_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetBinaryTypeValue(BinaryTypeValue value) {
	binary_type_value_.emplace(std::move(value));
	return *this;
}

PrimitiveTypeValue PrimitiveTypeValueBuilder::Build() {
	auto result = PrimitiveTypeValue(
	    std::move(boolean_type_value_), std::move(integer_type_value_), std::move(long_type_value_),
	    std::move(float_type_value_), std::move(double_type_value_), std::move(decimal_type_value_),
	    std::move(string_type_value_), std::move(uuidtype_value_), std::move(date_type_value_),
	    std::move(time_type_value_), std::move(timestamp_type_value_), std::move(timestamp_tz_type_value_),
	    std::move(timestamp_nano_type_value_), std::move(timestamp_tz_nano_type_value_), std::move(fixed_type_value_),
	    std::move(binary_type_value_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PrimitiveTypeValueBuilder::TryBuild(optional<PrimitiveTypeValue> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PrimitiveTypeValue PrimitiveTypeValue::FromJSON(yyjson_val *obj) {
	PrimitiveTypeValueBuilder builder;
	int matched_any_of_variants = 0;
	try {
		builder.SetBooleanTypeValue(BooleanTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetIntegerTypeValue(IntegerTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetLongTypeValue(LongTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetFloatTypeValue(FloatTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetDoubleTypeValue(DoubleTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetDecimalTypeValue(DecimalTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetStringTypeValue(StringTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetUuidtypeValue(UUIDTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetDateTypeValue(DateTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetTimeTypeValue(TimeTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetTimestampTypeValue(TimestampTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetTimestampTzTypeValue(TimestampTzTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetTimestampNanoTypeValue(TimestampNanoTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetTimestampTzNanoTypeValue(TimestampTzNanoTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetFixedTypeValue(FixedTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetBinaryTypeValue(BinaryTypeValue::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	if (matched_any_of_variants == 0) {
		throw InvalidInputException("PrimitiveTypeValue failed to parse, none of the anyOf candidates matched");
	}
	return builder.Build();
}

string PrimitiveTypeValue::TryFromJSON(yyjson_val *obj, optional<PrimitiveTypeValue> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PrimitiveTypeValue PrimitiveTypeValue::Copy() const {
	PrimitiveTypeValueBuilder builder;
	optional<BooleanTypeValue> boolean_type_value_tmp;
	if (boolean_type_value.has_value()) {
		boolean_type_value_tmp.emplace((*boolean_type_value).Copy());
	}
	if (boolean_type_value_tmp.has_value()) {
		builder.SetBooleanTypeValue(std::move(*boolean_type_value_tmp));
	}
	optional<IntegerTypeValue> integer_type_value_tmp;
	if (integer_type_value.has_value()) {
		integer_type_value_tmp.emplace((*integer_type_value).Copy());
	}
	if (integer_type_value_tmp.has_value()) {
		builder.SetIntegerTypeValue(std::move(*integer_type_value_tmp));
	}
	optional<LongTypeValue> long_type_value_tmp;
	if (long_type_value.has_value()) {
		long_type_value_tmp.emplace((*long_type_value).Copy());
	}
	if (long_type_value_tmp.has_value()) {
		builder.SetLongTypeValue(std::move(*long_type_value_tmp));
	}
	optional<FloatTypeValue> float_type_value_tmp;
	if (float_type_value.has_value()) {
		float_type_value_tmp.emplace((*float_type_value).Copy());
	}
	if (float_type_value_tmp.has_value()) {
		builder.SetFloatTypeValue(std::move(*float_type_value_tmp));
	}
	optional<DoubleTypeValue> double_type_value_tmp;
	if (double_type_value.has_value()) {
		double_type_value_tmp.emplace((*double_type_value).Copy());
	}
	if (double_type_value_tmp.has_value()) {
		builder.SetDoubleTypeValue(std::move(*double_type_value_tmp));
	}
	optional<DecimalTypeValue> decimal_type_value_tmp;
	if (decimal_type_value.has_value()) {
		decimal_type_value_tmp.emplace((*decimal_type_value).Copy());
	}
	if (decimal_type_value_tmp.has_value()) {
		builder.SetDecimalTypeValue(std::move(*decimal_type_value_tmp));
	}
	optional<StringTypeValue> string_type_value_tmp;
	if (string_type_value.has_value()) {
		string_type_value_tmp.emplace((*string_type_value).Copy());
	}
	if (string_type_value_tmp.has_value()) {
		builder.SetStringTypeValue(std::move(*string_type_value_tmp));
	}
	optional<UUIDTypeValue> uuidtype_value_tmp;
	if (uuidtype_value.has_value()) {
		uuidtype_value_tmp.emplace((*uuidtype_value).Copy());
	}
	if (uuidtype_value_tmp.has_value()) {
		builder.SetUuidtypeValue(std::move(*uuidtype_value_tmp));
	}
	optional<DateTypeValue> date_type_value_tmp;
	if (date_type_value.has_value()) {
		date_type_value_tmp.emplace((*date_type_value).Copy());
	}
	if (date_type_value_tmp.has_value()) {
		builder.SetDateTypeValue(std::move(*date_type_value_tmp));
	}
	optional<TimeTypeValue> time_type_value_tmp;
	if (time_type_value.has_value()) {
		time_type_value_tmp.emplace((*time_type_value).Copy());
	}
	if (time_type_value_tmp.has_value()) {
		builder.SetTimeTypeValue(std::move(*time_type_value_tmp));
	}
	optional<TimestampTypeValue> timestamp_type_value_tmp;
	if (timestamp_type_value.has_value()) {
		timestamp_type_value_tmp.emplace((*timestamp_type_value).Copy());
	}
	if (timestamp_type_value_tmp.has_value()) {
		builder.SetTimestampTypeValue(std::move(*timestamp_type_value_tmp));
	}
	optional<TimestampTzTypeValue> timestamp_tz_type_value_tmp;
	if (timestamp_tz_type_value.has_value()) {
		timestamp_tz_type_value_tmp.emplace((*timestamp_tz_type_value).Copy());
	}
	if (timestamp_tz_type_value_tmp.has_value()) {
		builder.SetTimestampTzTypeValue(std::move(*timestamp_tz_type_value_tmp));
	}
	optional<TimestampNanoTypeValue> timestamp_nano_type_value_tmp;
	if (timestamp_nano_type_value.has_value()) {
		timestamp_nano_type_value_tmp.emplace((*timestamp_nano_type_value).Copy());
	}
	if (timestamp_nano_type_value_tmp.has_value()) {
		builder.SetTimestampNanoTypeValue(std::move(*timestamp_nano_type_value_tmp));
	}
	optional<TimestampTzNanoTypeValue> timestamp_tz_nano_type_value_tmp;
	if (timestamp_tz_nano_type_value.has_value()) {
		timestamp_tz_nano_type_value_tmp.emplace((*timestamp_tz_nano_type_value).Copy());
	}
	if (timestamp_tz_nano_type_value_tmp.has_value()) {
		builder.SetTimestampTzNanoTypeValue(std::move(*timestamp_tz_nano_type_value_tmp));
	}
	optional<FixedTypeValue> fixed_type_value_tmp;
	if (fixed_type_value.has_value()) {
		fixed_type_value_tmp.emplace((*fixed_type_value).Copy());
	}
	if (fixed_type_value_tmp.has_value()) {
		builder.SetFixedTypeValue(std::move(*fixed_type_value_tmp));
	}
	optional<BinaryTypeValue> binary_type_value_tmp;
	if (binary_type_value.has_value()) {
		binary_type_value_tmp.emplace((*binary_type_value).Copy());
	}
	if (binary_type_value_tmp.has_value()) {
		builder.SetBinaryTypeValue(std::move(*binary_type_value_tmp));
	}
	return builder.Build();
}

string PrimitiveTypeValue::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (boolean_type_value.has_value()) {
		matched_any_of_variants++;
		error = boolean_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (integer_type_value.has_value()) {
		matched_any_of_variants++;
		error = integer_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (long_type_value.has_value()) {
		matched_any_of_variants++;
		error = long_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (float_type_value.has_value()) {
		matched_any_of_variants++;
		error = float_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (double_type_value.has_value()) {
		matched_any_of_variants++;
		error = double_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (decimal_type_value.has_value()) {
		matched_any_of_variants++;
		error = decimal_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (string_type_value.has_value()) {
		matched_any_of_variants++;
		error = string_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (uuidtype_value.has_value()) {
		matched_any_of_variants++;
		error = uuidtype_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (date_type_value.has_value()) {
		matched_any_of_variants++;
		error = date_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (time_type_value.has_value()) {
		matched_any_of_variants++;
		error = time_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (timestamp_type_value.has_value()) {
		matched_any_of_variants++;
		error = timestamp_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (timestamp_tz_type_value.has_value()) {
		matched_any_of_variants++;
		error = timestamp_tz_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (timestamp_nano_type_value.has_value()) {
		matched_any_of_variants++;
		error = timestamp_nano_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (timestamp_tz_nano_type_value.has_value()) {
		matched_any_of_variants++;
		error = timestamp_tz_nano_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (fixed_type_value.has_value()) {
		matched_any_of_variants++;
		error = fixed_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (binary_type_value.has_value()) {
		matched_any_of_variants++;
		error = binary_type_value->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "PrimitiveTypeValue must have at least one anyOf variant set";
	}
	return "";
}

yyjson_mut_val *PrimitiveTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	if (long_type_value.has_value()) {
		return long_type_value->ToJSON(doc);
	} else if (double_type_value.has_value()) {
		return double_type_value->ToJSON(doc);
	} else if (integer_type_value.has_value()) {
		return integer_type_value->ToJSON(doc);
	} else if (float_type_value.has_value()) {
		return float_type_value->ToJSON(doc);
	} else if (boolean_type_value.has_value()) {
		return boolean_type_value->ToJSON(doc);
	} else if (decimal_type_value.has_value()) {
		return decimal_type_value->ToJSON(doc);
	} else if (string_type_value.has_value()) {
		return string_type_value->ToJSON(doc);
	} else if (uuidtype_value.has_value()) {
		return uuidtype_value->ToJSON(doc);
	} else if (date_type_value.has_value()) {
		return date_type_value->ToJSON(doc);
	} else if (time_type_value.has_value()) {
		return time_type_value->ToJSON(doc);
	} else if (timestamp_type_value.has_value()) {
		return timestamp_type_value->ToJSON(doc);
	} else if (timestamp_tz_type_value.has_value()) {
		return timestamp_tz_type_value->ToJSON(doc);
	} else if (timestamp_nano_type_value.has_value()) {
		return timestamp_nano_type_value->ToJSON(doc);
	} else if (timestamp_tz_nano_type_value.has_value()) {
		return timestamp_tz_nano_type_value->ToJSON(doc);
	} else if (fixed_type_value.has_value()) {
		return fixed_type_value->ToJSON(doc);
	} else if (binary_type_value.has_value()) {
		return binary_type_value->ToJSON(doc);
	}
	// No variant is active - return null
	return yyjson_mut_null(doc);
}

} // namespace rest_api_objects
} // namespace duckdb
