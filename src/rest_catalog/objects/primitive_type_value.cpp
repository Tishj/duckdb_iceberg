
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
PrimitiveTypeValue::PrimitiveTypeValue(const PrimitiveTypeValue &other)
    : boolean_type_value((other.boolean_type_value.has_value()
                              ? optional<BooleanTypeValue>((*other.boolean_type_value).Copy())
                              : optional<BooleanTypeValue>())),
      integer_type_value((other.integer_type_value.has_value()
                              ? optional<IntegerTypeValue>((*other.integer_type_value).Copy())
                              : optional<IntegerTypeValue>())),
      long_type_value((other.long_type_value.has_value() ? optional<LongTypeValue>((*other.long_type_value).Copy())
                                                         : optional<LongTypeValue>())),
      float_type_value((other.float_type_value.has_value() ? optional<FloatTypeValue>((*other.float_type_value).Copy())
                                                           : optional<FloatTypeValue>())),
      double_type_value((other.double_type_value.has_value()
                             ? optional<DoubleTypeValue>((*other.double_type_value).Copy())
                             : optional<DoubleTypeValue>())),
      decimal_type_value((other.decimal_type_value.has_value()
                              ? optional<DecimalTypeValue>((*other.decimal_type_value).Copy())
                              : optional<DecimalTypeValue>())),
      string_type_value((other.string_type_value.has_value()
                             ? optional<StringTypeValue>((*other.string_type_value).Copy())
                             : optional<StringTypeValue>())),
      uuidtype_value((other.uuidtype_value.has_value() ? optional<UUIDTypeValue>((*other.uuidtype_value).Copy())
                                                       : optional<UUIDTypeValue>())),
      date_type_value((other.date_type_value.has_value() ? optional<DateTypeValue>((*other.date_type_value).Copy())
                                                         : optional<DateTypeValue>())),
      time_type_value((other.time_type_value.has_value() ? optional<TimeTypeValue>((*other.time_type_value).Copy())
                                                         : optional<TimeTypeValue>())),
      timestamp_type_value((other.timestamp_type_value.has_value()
                                ? optional<TimestampTypeValue>((*other.timestamp_type_value).Copy())
                                : optional<TimestampTypeValue>())),
      timestamp_tz_type_value((other.timestamp_tz_type_value.has_value()
                                   ? optional<TimestampTzTypeValue>((*other.timestamp_tz_type_value).Copy())
                                   : optional<TimestampTzTypeValue>())),
      timestamp_nano_type_value((other.timestamp_nano_type_value.has_value()
                                     ? optional<TimestampNanoTypeValue>((*other.timestamp_nano_type_value).Copy())
                                     : optional<TimestampNanoTypeValue>())),
      timestamp_tz_nano_type_value(
          (other.timestamp_tz_nano_type_value.has_value()
               ? optional<TimestampTzNanoTypeValue>((*other.timestamp_tz_nano_type_value).Copy())
               : optional<TimestampTzNanoTypeValue>())),
      fixed_type_value((other.fixed_type_value.has_value() ? optional<FixedTypeValue>((*other.fixed_type_value).Copy())
                                                           : optional<FixedTypeValue>())),
      binary_type_value((other.binary_type_value.has_value()
                             ? optional<BinaryTypeValue>((*other.binary_type_value).Copy())
                             : optional<BinaryTypeValue>())) {
}
PrimitiveTypeValue::PrimitiveTypeValue(PrimitiveTypeValue &&other)
    : PrimitiveTypeValue(static_cast<const PrimitiveTypeValue &>(other)) {
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

string PrimitiveTypeValue::TryFromJSON(yyjson_val *obj, PrimitiveTypeValueBuilder &builder) {
	try {
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
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PrimitiveTypeValue PrimitiveTypeValue::FromJSON(yyjson_val *obj) {
	PrimitiveTypeValueBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

PrimitiveTypeValue PrimitiveTypeValue::Copy() const {
	return PrimitiveTypeValue(*this);
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
