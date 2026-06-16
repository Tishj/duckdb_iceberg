
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/binary_type_value.hpp"
#include "rest_catalog/objects/boolean_type_value.hpp"
#include "rest_catalog/objects/date_type_value.hpp"
#include "rest_catalog/objects/decimal_type_value.hpp"
#include "rest_catalog/objects/double_type_value.hpp"
#include "rest_catalog/objects/fixed_type_value.hpp"
#include "rest_catalog/objects/float_type_value.hpp"
#include "rest_catalog/objects/integer_type_value.hpp"
#include "rest_catalog/objects/long_type_value.hpp"
#include "rest_catalog/objects/string_type_value.hpp"
#include "rest_catalog/objects/time_type_value.hpp"
#include "rest_catalog/objects/timestamp_nano_type_value.hpp"
#include "rest_catalog/objects/timestamp_type_value.hpp"
#include "rest_catalog/objects/timestamp_tz_nano_type_value.hpp"
#include "rest_catalog/objects/timestamp_tz_type_value.hpp"
#include "rest_catalog/objects/uuidtype_value.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class PrimitiveTypeValueBuilder;

class PrimitiveTypeValue {
public:
	PrimitiveTypeValue(const PrimitiveTypeValue &);
	PrimitiveTypeValue &operator=(const PrimitiveTypeValue &) = delete;
	PrimitiveTypeValue(PrimitiveTypeValue &&);
	PrimitiveTypeValue &operator=(PrimitiveTypeValue &&) = delete;

private:
	friend class PrimitiveTypeValueBuilder;
	PrimitiveTypeValue(optional<BooleanTypeValue> boolean_type_value_p, optional<IntegerTypeValue> integer_type_value_p,
	                   optional<LongTypeValue> long_type_value_p, optional<FloatTypeValue> float_type_value_p,
	                   optional<DoubleTypeValue> double_type_value_p, optional<DecimalTypeValue> decimal_type_value_p,
	                   optional<StringTypeValue> string_type_value_p, optional<UUIDTypeValue> uuidtype_value_p,
	                   optional<DateTypeValue> date_type_value_p, optional<TimeTypeValue> time_type_value_p,
	                   optional<TimestampTypeValue> timestamp_type_value_p,
	                   optional<TimestampTzTypeValue> timestamp_tz_type_value_p,
	                   optional<TimestampNanoTypeValue> timestamp_nano_type_value_p,
	                   optional<TimestampTzNanoTypeValue> timestamp_tz_nano_type_value_p,
	                   optional<FixedTypeValue> fixed_type_value_p, optional<BinaryTypeValue> binary_type_value_p);

public:
	// Deserialization
	static PrimitiveTypeValue FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, PrimitiveTypeValueBuilder &builder);
	optional<string> Validate() const;

	// Copy
	PrimitiveTypeValue Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<BooleanTypeValue> boolean_type_value;
	const optional<IntegerTypeValue> integer_type_value;
	const optional<LongTypeValue> long_type_value;
	const optional<FloatTypeValue> float_type_value;
	const optional<DoubleTypeValue> double_type_value;
	const optional<DecimalTypeValue> decimal_type_value;
	const optional<StringTypeValue> string_type_value;
	const optional<UUIDTypeValue> uuidtype_value;
	const optional<DateTypeValue> date_type_value;
	const optional<TimeTypeValue> time_type_value;
	const optional<TimestampTypeValue> timestamp_type_value;
	const optional<TimestampTzTypeValue> timestamp_tz_type_value;
	const optional<TimestampNanoTypeValue> timestamp_nano_type_value;
	const optional<TimestampTzNanoTypeValue> timestamp_tz_nano_type_value;
	const optional<FixedTypeValue> fixed_type_value;
	const optional<BinaryTypeValue> binary_type_value;
};

class PrimitiveTypeValueBuilder {
public:
	PrimitiveTypeValueBuilder();
	PrimitiveTypeValueBuilder &SetBooleanTypeValue(BooleanTypeValue value);
	PrimitiveTypeValueBuilder &SetIntegerTypeValue(IntegerTypeValue value);
	PrimitiveTypeValueBuilder &SetLongTypeValue(LongTypeValue value);
	PrimitiveTypeValueBuilder &SetFloatTypeValue(FloatTypeValue value);
	PrimitiveTypeValueBuilder &SetDoubleTypeValue(DoubleTypeValue value);
	PrimitiveTypeValueBuilder &SetDecimalTypeValue(DecimalTypeValue value);
	PrimitiveTypeValueBuilder &SetStringTypeValue(StringTypeValue value);
	PrimitiveTypeValueBuilder &SetUuidtypeValue(UUIDTypeValue value);
	PrimitiveTypeValueBuilder &SetDateTypeValue(DateTypeValue value);
	PrimitiveTypeValueBuilder &SetTimeTypeValue(TimeTypeValue value);
	PrimitiveTypeValueBuilder &SetTimestampTypeValue(TimestampTypeValue value);
	PrimitiveTypeValueBuilder &SetTimestampTzTypeValue(TimestampTzTypeValue value);
	PrimitiveTypeValueBuilder &SetTimestampNanoTypeValue(TimestampNanoTypeValue value);
	PrimitiveTypeValueBuilder &SetTimestampTzNanoTypeValue(TimestampTzNanoTypeValue value);
	PrimitiveTypeValueBuilder &SetFixedTypeValue(FixedTypeValue value);
	PrimitiveTypeValueBuilder &SetBinaryTypeValue(BinaryTypeValue value);
	optional<string> TryBuild(optional<PrimitiveTypeValue> &result);
	PrimitiveTypeValue Build();

private:
	optional<BooleanTypeValue> boolean_type_value_;
	optional<IntegerTypeValue> integer_type_value_;
	optional<LongTypeValue> long_type_value_;
	optional<FloatTypeValue> float_type_value_;
	optional<DoubleTypeValue> double_type_value_;
	optional<DecimalTypeValue> decimal_type_value_;
	optional<StringTypeValue> string_type_value_;
	optional<UUIDTypeValue> uuidtype_value_;
	optional<DateTypeValue> date_type_value_;
	optional<TimeTypeValue> time_type_value_;
	optional<TimestampTypeValue> timestamp_type_value_;
	optional<TimestampTzTypeValue> timestamp_tz_type_value_;
	optional<TimestampNanoTypeValue> timestamp_nano_type_value_;
	optional<TimestampTzNanoTypeValue> timestamp_tz_nano_type_value_;
	optional<FixedTypeValue> fixed_type_value_;
	optional<BinaryTypeValue> binary_type_value_;
};

} // namespace rest_api_objects
} // namespace duckdb
