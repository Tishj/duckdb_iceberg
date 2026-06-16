
#include "rest_catalog/objects/base_update.hpp"

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

BaseUpdate::BaseUpdate(string action_p) : action(std::move(action_p)) {
}

BaseUpdateBuilder::BaseUpdateBuilder() {
}

BaseUpdateBuilder &BaseUpdateBuilder::SetAction(string value) {
	action_.emplace(std::move(value));
	has_action_ = true;
	return *this;
}

BaseUpdate BaseUpdateBuilder::Build() {
	if (!has_action_) {
		throw InvalidInputException("BaseUpdate required property 'action' is missing");
	}
	auto result = BaseUpdate(std::move(*action_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string BaseUpdateBuilder::TryBuild(optional<BaseUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string BaseUpdate::TryFromJSON(yyjson_val *obj, BaseUpdateBuilder &builder) {
	try {
		auto action_val = yyjson_obj_get(obj, "action");
		if (!action_val) {
			throw InvalidInputException("BaseUpdate required property 'action' is missing");
		} else {
			string action;
			if (yyjson_is_str(action_val)) {
				action = yyjson_get_str(action_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("BaseUpdate property 'action' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(action_val)));
			}
			builder.SetAction(std::move(action));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

BaseUpdate BaseUpdate::FromJSON(yyjson_val *obj) {
	BaseUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

BaseUpdate BaseUpdate::Copy() const {
	BaseUpdateBuilder builder;
	string action_tmp;
	action_tmp = action;
	builder.SetAction(std::move(action_tmp));
	return builder.Build();
}

string BaseUpdate::Validate() const {
	string error;
	return "";
}

void BaseUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: action
	yyjson_mut_obj_add_strcpy(doc, obj, "action", action.c_str());
}

yyjson_mut_val *BaseUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
