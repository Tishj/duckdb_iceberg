
#include "rest_catalog/objects/set_properties_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SetPropertiesUpdate::SetPropertiesUpdate() {
}

SetPropertiesUpdateBuilder::SetPropertiesUpdateBuilder() {
}

SetPropertiesUpdateBuilder &SetPropertiesUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

SetPropertiesUpdateBuilder &SetPropertiesUpdateBuilder::SetUpdates(case_insensitive_map_t<string> value) {
	result_.updates = std::move(value);
	has_updates_ = true;
	return *this;
}

string SetPropertiesUpdateBuilder::TryBuild(SetPropertiesUpdate &result) {
	if (!has_updates_) {
		return "SetPropertiesUpdate required property 'updates' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

SetPropertiesUpdate SetPropertiesUpdateBuilder::Build() {
	SetPropertiesUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

SetPropertiesUpdate SetPropertiesUpdate::FromJSON(yyjson_val *obj) {
	SetPropertiesUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

SetPropertiesUpdate SetPropertiesUpdate::Copy() const {
	SetPropertiesUpdate res;
	res.base_update = base_update.Copy();
	for (auto &entry : updates) {
		res.updates.emplace(entry.first, entry.second);
	}
	return res;
}

string SetPropertiesUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string SetPropertiesUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto updates_val = yyjson_obj_get(obj, "updates");
	if (!updates_val) {
		return "SetPropertiesUpdate required property 'updates' is missing";
	} else {
		if (yyjson_is_obj(updates_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(updates_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					return StringUtil::Format(
					    "SetPropertiesUpdate property 'tmp' is not of type 'string', found '%s' instead",
					    yyjson_get_type_desc(val));
				}
				updates.emplace(key_str, std::move(tmp));
			}
		} else {
			return "SetPropertiesUpdate property 'updates' is not of type 'object'";
		}
	}
	return Validate();
}

void SetPropertiesUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: updates
	yyjson_mut_val *updates_obj = yyjson_mut_obj(doc);
	for (const auto &it : updates) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, updates_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "updates", updates_obj);
}

yyjson_mut_val *SetPropertiesUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
