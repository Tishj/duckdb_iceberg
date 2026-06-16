
#include "rest_catalog/objects/long_type_value.hpp"

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

LongTypeValue::LongTypeValue(int64_t value_p) : value(std::move(value_p)) {
}

string LongTypeValue::TryFromJSON(yyjson_val *obj, optional<LongTypeValue> &result) {
	try {
		int64_t value;
		if (yyjson_is_sint(obj)) {
			value = yyjson_get_sint(obj);
		} else if (yyjson_is_uint(obj)) {
			value = yyjson_get_uint(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("LongTypeValue property 'value' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(LongTypeValue(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LongTypeValue LongTypeValue::FromJSON(yyjson_val *obj) {
	optional<LongTypeValue> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

LongTypeValue LongTypeValue::Copy() const {
	int64_t value_tmp;
	value_tmp = value;
	return LongTypeValue(std::move(value_tmp));
}

string LongTypeValue::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *LongTypeValue::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_sint(doc, value);
}

} // namespace rest_api_objects
} // namespace duckdb
