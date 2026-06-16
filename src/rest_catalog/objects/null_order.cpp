
#include "rest_catalog/objects/null_order.hpp"

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

NullOrder::NullOrder(string value_p) : value(std::move(value_p)) {
}
NullOrder::NullOrder(const NullOrder &other) : value(other.value) {
}
NullOrder::NullOrder(NullOrder &&other) : NullOrder(static_cast<const NullOrder &>(other)) {
}

string NullOrder::TryFromJSON(yyjson_val *obj, optional<NullOrder> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "NullOrder property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(NullOrder(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

NullOrder NullOrder::FromJSON(yyjson_val *obj) {
	optional<NullOrder> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

NullOrder NullOrder::Copy() const {
	return NullOrder(*this);
}

string NullOrder::Validate() const {
	string error;
	if (!StringUtil::CIEquals(value, "nulls-first") && !StringUtil::CIEquals(value, "nulls-last")) {
		return StringUtil::Format("NullOrder property 'value' must be one of [nulls-first, nulls-last], not %s", value);
	}
	return "";
}

yyjson_mut_val *NullOrder::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
