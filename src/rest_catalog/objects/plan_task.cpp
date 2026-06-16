
#include "rest_catalog/objects/plan_task.hpp"

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

PlanTask::PlanTask(string value_p) : value(std::move(value_p)) {
}
PlanTask::PlanTask(const PlanTask &other) : value(other.value) {
}
PlanTask::PlanTask(PlanTask &&other) : PlanTask(static_cast<const PlanTask &>(other)) {
}

optional<string> PlanTask::TryFromJSON(yyjson_val *obj, optional<PlanTask> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "PlanTask property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(PlanTask(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanTask PlanTask::FromJSON(yyjson_val *obj) {
	optional<PlanTask> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

PlanTask PlanTask::Copy() const {
	return PlanTask(*this);
}

optional<string> PlanTask::Validate() const {
	optional<string> error;
	return nullopt;
}

yyjson_mut_val *PlanTask::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
