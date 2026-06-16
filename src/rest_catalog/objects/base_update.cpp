
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
BaseUpdate::BaseUpdate(const BaseUpdate &other) : action(other.action) {
}
BaseUpdate::BaseUpdate(BaseUpdate &&other) : BaseUpdate(static_cast<const BaseUpdate &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> BaseUpdateBuilder::TryBuild(optional<BaseUpdate> &result) {
	if (!has_action_) {
		return "BaseUpdate required property 'action' is missing";
	}
	auto built = BaseUpdate(std::move(*action_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> BaseUpdate::TryFromJSON(yyjson_val *obj, BaseUpdateBuilder &builder) {
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

BaseUpdate BaseUpdate::FromJSON(yyjson_val *obj) {
	BaseUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

BaseUpdate BaseUpdate::Copy() const {
	return BaseUpdate(*this);
}

optional<string> BaseUpdate::Validate() const {
	optional<string> error;
	return nullopt;
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
