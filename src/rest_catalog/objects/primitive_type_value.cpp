
#include "rest_catalog/objects/primitive_type_value.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

PrimitiveTypeValue::PrimitiveTypeValue() {
}

PrimitiveTypeValueBuilder::PrimitiveTypeValueBuilder() {
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetBooleanTypeValue(BooleanTypeValue value) {
	result_.boolean_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetIntegerTypeValue(IntegerTypeValue value) {
	result_.integer_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetLongTypeValue(LongTypeValue value) {
	result_.long_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetFloatTypeValue(FloatTypeValue value) {
	result_.float_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDoubleTypeValue(DoubleTypeValue value) {
	result_.double_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDecimalTypeValue(DecimalTypeValue value) {
	result_.decimal_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetStringTypeValue(StringTypeValue value) {
	result_.string_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetUuidtypeValue(UUIDTypeValue value) {
	result_.uuidtype_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetDateTypeValue(DateTypeValue value) {
	result_.date_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimeTypeValue(TimeTypeValue value) {
	result_.time_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTypeValue(TimestampTypeValue value) {
	result_.timestamp_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTzTypeValue(TimestampTzTypeValue value) {
	result_.timestamp_tz_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampNanoTypeValue(TimestampNanoTypeValue value) {
	result_.timestamp_nano_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetTimestampTzNanoTypeValue(TimestampTzNanoTypeValue value) {
	result_.timestamp_tz_nano_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetFixedTypeValue(FixedTypeValue value) {
	result_.fixed_type_value = std::move(value);
	return *this;
}

PrimitiveTypeValueBuilder &PrimitiveTypeValueBuilder::SetBinaryTypeValue(BinaryTypeValue value) {
	result_.binary_type_value = std::move(value);
	return *this;
}

string PrimitiveTypeValueBuilder::TryBuild(PrimitiveTypeValue &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

PrimitiveTypeValue PrimitiveTypeValueBuilder::Build() {
	PrimitiveTypeValue result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

PrimitiveTypeValue PrimitiveTypeValue::FromJSON(yyjson_val *obj) {
	PrimitiveTypeValue res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

PrimitiveTypeValue PrimitiveTypeValue::Copy() const {
	PrimitiveTypeValue res;
	if (boolean_type_value.has_value()) {
		res.boolean_type_value.emplace();
		(*res.boolean_type_value) = (*boolean_type_value).Copy();
	}
	if (integer_type_value.has_value()) {
		res.integer_type_value.emplace();
		(*res.integer_type_value) = (*integer_type_value).Copy();
	}
	if (long_type_value.has_value()) {
		res.long_type_value.emplace();
		(*res.long_type_value) = (*long_type_value).Copy();
	}
	if (float_type_value.has_value()) {
		res.float_type_value.emplace();
		(*res.float_type_value) = (*float_type_value).Copy();
	}
	if (double_type_value.has_value()) {
		res.double_type_value.emplace();
		(*res.double_type_value) = (*double_type_value).Copy();
	}
	if (decimal_type_value.has_value()) {
		res.decimal_type_value.emplace();
		(*res.decimal_type_value) = (*decimal_type_value).Copy();
	}
	if (string_type_value.has_value()) {
		res.string_type_value.emplace();
		(*res.string_type_value) = (*string_type_value).Copy();
	}
	if (uuidtype_value.has_value()) {
		res.uuidtype_value.emplace();
		(*res.uuidtype_value) = (*uuidtype_value).Copy();
	}
	if (date_type_value.has_value()) {
		res.date_type_value.emplace();
		(*res.date_type_value) = (*date_type_value).Copy();
	}
	if (time_type_value.has_value()) {
		res.time_type_value.emplace();
		(*res.time_type_value) = (*time_type_value).Copy();
	}
	if (timestamp_type_value.has_value()) {
		res.timestamp_type_value.emplace();
		(*res.timestamp_type_value) = (*timestamp_type_value).Copy();
	}
	if (timestamp_tz_type_value.has_value()) {
		res.timestamp_tz_type_value.emplace();
		(*res.timestamp_tz_type_value) = (*timestamp_tz_type_value).Copy();
	}
	if (timestamp_nano_type_value.has_value()) {
		res.timestamp_nano_type_value.emplace();
		(*res.timestamp_nano_type_value) = (*timestamp_nano_type_value).Copy();
	}
	if (timestamp_tz_nano_type_value.has_value()) {
		res.timestamp_tz_nano_type_value.emplace();
		(*res.timestamp_tz_nano_type_value) = (*timestamp_tz_nano_type_value).Copy();
	}
	if (fixed_type_value.has_value()) {
		res.fixed_type_value.emplace();
		(*res.fixed_type_value) = (*fixed_type_value).Copy();
	}
	if (binary_type_value.has_value()) {
		res.binary_type_value.emplace();
		(*res.binary_type_value) = (*binary_type_value).Copy();
	}
	return res;
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

string PrimitiveTypeValue::TryFromJSON(yyjson_val *obj) {
	string error;
	boolean_type_value.emplace();
	error = boolean_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		boolean_type_value = nullopt;
	}
	integer_type_value.emplace();
	error = integer_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		integer_type_value = nullopt;
	}
	long_type_value.emplace();
	error = long_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		long_type_value = nullopt;
	}
	float_type_value.emplace();
	error = float_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		float_type_value = nullopt;
	}
	double_type_value.emplace();
	error = double_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		double_type_value = nullopt;
	}
	decimal_type_value.emplace();
	error = decimal_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		decimal_type_value = nullopt;
	}
	string_type_value.emplace();
	error = string_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		string_type_value = nullopt;
	}
	uuidtype_value.emplace();
	error = uuidtype_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		uuidtype_value = nullopt;
	}
	date_type_value.emplace();
	error = date_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		date_type_value = nullopt;
	}
	time_type_value.emplace();
	error = time_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		time_type_value = nullopt;
	}
	timestamp_type_value.emplace();
	error = timestamp_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		timestamp_type_value = nullopt;
	}
	timestamp_tz_type_value.emplace();
	error = timestamp_tz_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		timestamp_tz_type_value = nullopt;
	}
	timestamp_nano_type_value.emplace();
	error = timestamp_nano_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		timestamp_nano_type_value = nullopt;
	}
	timestamp_tz_nano_type_value.emplace();
	error = timestamp_tz_nano_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		timestamp_tz_nano_type_value = nullopt;
	}
	fixed_type_value.emplace();
	error = fixed_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		fixed_type_value = nullopt;
	}
	binary_type_value.emplace();
	error = binary_type_value->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		binary_type_value = nullopt;
	}
	if (!(binary_type_value.has_value()) && !(boolean_type_value.has_value()) && !(date_type_value.has_value()) &&
	    !(decimal_type_value.has_value()) && !(double_type_value.has_value()) && !(fixed_type_value.has_value()) &&
	    !(float_type_value.has_value()) && !(integer_type_value.has_value()) && !(long_type_value.has_value()) &&
	    !(string_type_value.has_value()) && !(time_type_value.has_value()) &&
	    !(timestamp_nano_type_value.has_value()) && !(timestamp_type_value.has_value()) &&
	    !(timestamp_tz_nano_type_value.has_value()) && !(timestamp_tz_type_value.has_value()) &&
	    !(uuidtype_value.has_value())) {
		return "PrimitiveTypeValue failed to parse, none of the anyOf candidates matched";
	}
	return Validate();
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
