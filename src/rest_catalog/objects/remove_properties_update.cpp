
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

RemovePropertiesUpdate::RemovePropertiesUpdate(BaseUpdate base_update_p, vector<string> removals_p)
    : base_update(std::move(base_update_p)), removals(std::move(removals_p)) {
}

RemovePropertiesUpdateBuilder::RemovePropertiesUpdateBuilder() {
}

RemovePropertiesUpdateBuilder &RemovePropertiesUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

RemovePropertiesUpdateBuilder &RemovePropertiesUpdateBuilder::SetRemovals(vector<string> value) {
	removals_ = std::move(value);
	has_removals_ = true;
	return *this;
}

RemovePropertiesUpdate RemovePropertiesUpdateBuilder::Build() {
	if (!has_removals_) {
		throw InvalidInputException("RemovePropertiesUpdate required property 'removals' is missing");
	}
	auto result = RemovePropertiesUpdate(std::move(*base_update_), std::move(*removals_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemovePropertiesUpdateBuilder::TryBuild(optional<RemovePropertiesUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemovePropertiesUpdate RemovePropertiesUpdate::FromJSON(yyjson_val *obj) {
	RemovePropertiesUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto removals_val = yyjson_obj_get(obj, "removals");
	if (!removals_val) {
		throw InvalidInputException("RemovePropertiesUpdate required property 'removals' is missing");
	} else {
		vector<string> removals;
		if (yyjson_is_arr(removals_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(removals_val, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(StringUtil::Format(
					    "RemovePropertiesUpdate property 'tmp' is not of type 'string', found '%s' instead",
					    yyjson_get_type_desc(val)));
				}
				removals.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "RemovePropertiesUpdate property 'removals' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(removals_val));
		}
		builder.SetRemovals(std::move(removals));
	}
	return builder.Build();
}

string RemovePropertiesUpdate::TryFromJSON(yyjson_val *obj, optional<RemovePropertiesUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemovePropertiesUpdate RemovePropertiesUpdate::Copy() const {
	RemovePropertiesUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	vector<string> removals_tmp;
	removals_tmp.reserve(removals.size());
	for (auto &item : removals) {
		removals_tmp.emplace_back(item);
	}
	builder.SetRemovals(std::move(removals_tmp));
	return builder.Build();
}

string RemovePropertiesUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
