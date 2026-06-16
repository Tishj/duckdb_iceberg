
#include "rest_catalog/objects/update_namespace_properties_request.hpp"

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

UpdateNamespacePropertiesRequest::UpdateNamespacePropertiesRequest(optional<vector<string>> removals_p,
                                                                   optional<case_insensitive_map_t<string>> updates_p)
    : removals(std::move(removals_p)), updates(std::move(updates_p)) {
}
UpdateNamespacePropertiesRequest::UpdateNamespacePropertiesRequest(const UpdateNamespacePropertiesRequest &other)
    : removals((other.removals.has_value() ? optional<vector<string>>(([&]() {
	      vector<string> copied;
	      copied.reserve((*other.removals).size());
	      for (const auto &item : (*other.removals)) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }()))
                                           : optional<vector<string>>())),
      updates((other.updates.has_value() ? optional<case_insensitive_map_t<string>>(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : (*other.updates)) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }()))
                                         : optional<case_insensitive_map_t<string>>())) {
}
UpdateNamespacePropertiesRequest::UpdateNamespacePropertiesRequest(UpdateNamespacePropertiesRequest &&other)
    : UpdateNamespacePropertiesRequest(static_cast<const UpdateNamespacePropertiesRequest &>(other)) {
}

UpdateNamespacePropertiesRequestBuilder::UpdateNamespacePropertiesRequestBuilder() {
}

UpdateNamespacePropertiesRequestBuilder &UpdateNamespacePropertiesRequestBuilder::SetRemovals(vector<string> value) {
	removals_.emplace(std::move(value));
	return *this;
}

UpdateNamespacePropertiesRequestBuilder &
UpdateNamespacePropertiesRequestBuilder::SetUpdates(case_insensitive_map_t<string> value) {
	updates_.emplace(std::move(value));
	return *this;
}

UpdateNamespacePropertiesRequest UpdateNamespacePropertiesRequestBuilder::Build() {
	auto result = UpdateNamespacePropertiesRequest(std::move(removals_), std::move(updates_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string UpdateNamespacePropertiesRequestBuilder::TryBuild(optional<UpdateNamespacePropertiesRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string UpdateNamespacePropertiesRequest::TryFromJSON(yyjson_val *obj,
                                                     UpdateNamespacePropertiesRequestBuilder &builder) {
	try {
		auto removals_val = yyjson_obj_get(obj, "removals");
		if (removals_val) {
			vector<string> removals;
			if (yyjson_is_arr(removals_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(removals_val, idx, max, val) {
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(
						    StringUtil::Format("UpdateNamespacePropertiesRequest property 'tmp' is not of type "
						                       "'string', found '%s' instead",
						                       yyjson_get_type_desc(val)));
					}
					removals.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "UpdateNamespacePropertiesRequest property 'removals' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(removals_val)));
			}
			builder.SetRemovals(std::move(removals));
		}
		auto updates_val = yyjson_obj_get(obj, "updates");
		if (updates_val) {
			case_insensitive_map_t<string> updates;
			if (yyjson_is_obj(updates_val)) {
				size_t idx, max;
				yyjson_val *key, *val;
				yyjson_obj_foreach(updates_val, idx, max, key, val) {
					auto key_str = yyjson_get_str(key);
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(
						    StringUtil::Format("UpdateNamespacePropertiesRequest property 'tmp' is not of type "
						                       "'string', found '%s' instead",
						                       yyjson_get_type_desc(val)));
					}
					updates.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    "UpdateNamespacePropertiesRequest property 'updates' is not of type 'object'");
			}
			builder.SetUpdates(std::move(updates));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UpdateNamespacePropertiesRequest UpdateNamespacePropertiesRequest::FromJSON(yyjson_val *obj) {
	UpdateNamespacePropertiesRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

UpdateNamespacePropertiesRequest UpdateNamespacePropertiesRequest::Copy() const {
	return UpdateNamespacePropertiesRequest(*this);
}

string UpdateNamespacePropertiesRequest::Validate() const {
	string error;
	return "";
}

void UpdateNamespacePropertiesRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: removals
	if (removals.has_value()) {
		auto &removals_value = *removals;
		yyjson_mut_val *removals_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : removals_value) {
			yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
			yyjson_mut_arr_append(removals_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "removals", removals_value_arr);
	}

	// Serialize: updates
	if (updates.has_value()) {
		auto &updates_value = *updates;
		yyjson_mut_val *updates_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : updates_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, updates_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "updates", updates_value_obj);
	}
}

yyjson_mut_val *UpdateNamespacePropertiesRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
