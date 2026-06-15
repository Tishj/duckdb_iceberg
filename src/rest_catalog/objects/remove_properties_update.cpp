
#include "rest_catalog/objects/remove_properties_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RemovePropertiesUpdate::RemovePropertiesUpdate() {
}

RemovePropertiesUpdateBuilder::RemovePropertiesUpdateBuilder() {
}

RemovePropertiesUpdateBuilder &RemovePropertiesUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

RemovePropertiesUpdateBuilder &RemovePropertiesUpdateBuilder::SetRemovals(vector<string> value) {
	result_.removals = std::move(value);
	has_removals_ = true;
	return *this;
}

string RemovePropertiesUpdateBuilder::TryBuild(RemovePropertiesUpdate &result) {
	if (!has_removals_) {
		return "RemovePropertiesUpdate required property 'removals' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RemovePropertiesUpdate RemovePropertiesUpdateBuilder::Build() {
	RemovePropertiesUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RemovePropertiesUpdate RemovePropertiesUpdate::FromJSON(yyjson_val *obj) {
	RemovePropertiesUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RemovePropertiesUpdate RemovePropertiesUpdate::Copy() const {
	RemovePropertiesUpdate res;
	res.base_update = base_update.Copy();
	res.removals.reserve(removals.size());
	for (auto &item : removals) {
		res.removals.emplace_back(item);
	}
	return res;
}

string RemovePropertiesUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string RemovePropertiesUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto removals_val = yyjson_obj_get(obj, "removals");
	if (!removals_val) {
		return "RemovePropertiesUpdate required property 'removals' is missing";
	} else {
		if (yyjson_is_arr(removals_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(removals_val, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					return StringUtil::Format(
					    "RemovePropertiesUpdate property 'tmp' is not of type 'string', found '%s' instead",
					    yyjson_get_type_desc(val));
				}
				removals.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "RemovePropertiesUpdate property 'removals' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(removals_val));
		}
	}
	return Validate();
}

void RemovePropertiesUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: removals
	yyjson_mut_val *removals_arr = yyjson_mut_arr(doc);
	for (const auto &item : removals) {
		yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
		yyjson_mut_arr_append(removals_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "removals", removals_arr);
}

yyjson_mut_val *RemovePropertiesUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
