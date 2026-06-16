
#include "rest_catalog/objects/plan_status.hpp"

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

PlanStatus::PlanStatus(string value_p) : value(std::move(value_p)) {
}

string PlanStatus::TryFromJSON(yyjson_val *obj, optional<PlanStatus> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "PlanStatus property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(PlanStatus(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanStatus PlanStatus::FromJSON(yyjson_val *obj) {
	optional<PlanStatus> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

PlanStatus PlanStatus::Copy() const {
	string value_tmp;
	value_tmp = value;
	return PlanStatus(std::move(value_tmp));
}

string PlanStatus::Validate() const {
	string error;
	if (!StringUtil::CIEquals(value, "completed") && !StringUtil::CIEquals(value, "submitted") &&
	    !StringUtil::CIEquals(value, "cancelled") && !StringUtil::CIEquals(value, "failed")) {
		return StringUtil::Format(
		    "PlanStatus property 'value' must be one of [completed, submitted, cancelled, failed], not %s", value);
	}
	return "";
}

yyjson_mut_val *PlanStatus::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
