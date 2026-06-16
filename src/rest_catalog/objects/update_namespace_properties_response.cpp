
#include "rest_catalog/objects/update_namespace_properties_response.hpp"

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

UpdateNamespacePropertiesResponse::UpdateNamespacePropertiesResponse(vector<string> updated_p, vector<string> removed_p,
                                                                     optional<vector<string>> missing_p)
    : updated(std::move(updated_p)), removed(std::move(removed_p)), missing(std::move(missing_p)) {
}
UpdateNamespacePropertiesResponse::UpdateNamespacePropertiesResponse(const UpdateNamespacePropertiesResponse &other)
    : updated(([&]() {
	      vector<string> copied;
	      copied.reserve(other.updated.size());
	      for (const auto &item : other.updated) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())),
      removed(([&]() {
	      vector<string> copied;
	      copied.reserve(other.removed.size());
	      for (const auto &item : other.removed) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())),
      missing((other.missing.has_value() ? optional<vector<string>>(([&]() {
	      vector<string> copied;
	      copied.reserve((*other.missing).size());
	      for (const auto &item : (*other.missing)) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }()))
                                         : optional<vector<string>>())) {
}
UpdateNamespacePropertiesResponse::UpdateNamespacePropertiesResponse(UpdateNamespacePropertiesResponse &&other)
    : UpdateNamespacePropertiesResponse(static_cast<const UpdateNamespacePropertiesResponse &>(other)) {
}

UpdateNamespacePropertiesResponseBuilder::UpdateNamespacePropertiesResponseBuilder() {
}

UpdateNamespacePropertiesResponseBuilder &UpdateNamespacePropertiesResponseBuilder::SetUpdated(vector<string> value) {
	updated_.emplace(std::move(value));
	has_updated_ = true;
	return *this;
}

UpdateNamespacePropertiesResponseBuilder &UpdateNamespacePropertiesResponseBuilder::SetRemoved(vector<string> value) {
	removed_.emplace(std::move(value));
	has_removed_ = true;
	return *this;
}

UpdateNamespacePropertiesResponseBuilder &UpdateNamespacePropertiesResponseBuilder::SetMissing(vector<string> value) {
	missing_.emplace(std::move(value));
	return *this;
}

UpdateNamespacePropertiesResponse UpdateNamespacePropertiesResponseBuilder::Build() {
	if (!has_updated_) {
		throw InvalidInputException("UpdateNamespacePropertiesResponse required property 'updated' is missing");
	}
	if (!has_removed_) {
		throw InvalidInputException("UpdateNamespacePropertiesResponse required property 'removed' is missing");
	}
	auto result = UpdateNamespacePropertiesResponse(std::move(*updated_), std::move(*removed_), std::move(missing_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string>
UpdateNamespacePropertiesResponseBuilder::TryBuild(optional<UpdateNamespacePropertiesResponse> &result) {
	if (!has_updated_) {
		return "UpdateNamespacePropertiesResponse required property 'updated' is missing";
	}
	if (!has_removed_) {
		return "UpdateNamespacePropertiesResponse required property 'removed' is missing";
	}
	auto built = UpdateNamespacePropertiesResponse(std::move(*updated_), std::move(*removed_), std::move(missing_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> UpdateNamespacePropertiesResponse::TryFromJSON(yyjson_val *obj,
                                                                UpdateNamespacePropertiesResponseBuilder &builder) {
	try {
		auto updated_val = yyjson_obj_get(obj, "updated");
		if (!updated_val) {
			throw InvalidInputException("UpdateNamespacePropertiesResponse required property 'updated' is missing");
		} else {
			vector<string> updated;
			if (yyjson_is_arr(updated_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(updated_val, idx, max, val) {
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(
						    StringUtil::Format("UpdateNamespacePropertiesResponse property 'tmp' is not of type "
						                       "'string', found '%s' instead",
						                       yyjson_get_type_desc(val)));
					}
					updated.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "UpdateNamespacePropertiesResponse property 'updated' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(updated_val)));
			}
			builder.SetUpdated(std::move(updated));
		}
		auto removed_val = yyjson_obj_get(obj, "removed");
		if (!removed_val) {
			throw InvalidInputException("UpdateNamespacePropertiesResponse required property 'removed' is missing");
		} else {
			vector<string> removed;
			if (yyjson_is_arr(removed_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(removed_val, idx, max, val) {
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(
						    StringUtil::Format("UpdateNamespacePropertiesResponse property 'tmp' is not of type "
						                       "'string', found '%s' instead",
						                       yyjson_get_type_desc(val)));
					}
					removed.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "UpdateNamespacePropertiesResponse property 'removed' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(removed_val)));
			}
			builder.SetRemoved(std::move(removed));
		}
		auto missing_val = yyjson_obj_get(obj, "missing");
		if (missing_val) {
			if (yyjson_is_null(missing_val)) {
				//! do nothing, property is explicitly nullable
			} else {
				vector<string> missing;
				if (yyjson_is_arr(missing_val)) {
					size_t idx, max;
					yyjson_val *val;
					yyjson_arr_foreach(missing_val, idx, max, val) {
						string tmp;
						if (yyjson_is_str(val)) {
							tmp = yyjson_get_str(val);
						} else {
							throw InvalidInputException(
							    StringUtil::Format("UpdateNamespacePropertiesResponse property 'tmp' is not of type "
							                       "'string', found '%s' instead",
							                       yyjson_get_type_desc(val)));
						}
						missing.emplace_back(std::move(tmp));
					}
				} else {
					throw InvalidInputException(
					    StringUtil::Format("UpdateNamespacePropertiesResponse property 'missing' is not of type "
					                       "'array', found '%s' instead",
					                       yyjson_get_type_desc(missing_val)));
				}
				builder.SetMissing(std::move(missing));
			}
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UpdateNamespacePropertiesResponse UpdateNamespacePropertiesResponse::FromJSON(yyjson_val *obj) {
	UpdateNamespacePropertiesResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

UpdateNamespacePropertiesResponse UpdateNamespacePropertiesResponse::Copy() const {
	return UpdateNamespacePropertiesResponse(*this);
}

optional<string> UpdateNamespacePropertiesResponse::Validate() const {
	optional<string> error;
	return nullopt;
}

void UpdateNamespacePropertiesResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: updated
	yyjson_mut_val *updated_arr = yyjson_mut_arr(doc);
	for (const auto &item : updated) {
		yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
		yyjson_mut_arr_append(updated_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "updated", updated_arr);

	// Serialize: removed
	yyjson_mut_val *removed_arr = yyjson_mut_arr(doc);
	for (const auto &item : removed) {
		yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
		yyjson_mut_arr_append(removed_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "removed", removed_arr);

	// Serialize: missing
	if (missing.has_value()) {
		auto &missing_value = *missing;
		yyjson_mut_val *missing_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : missing_value) {
			yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
			yyjson_mut_arr_append(missing_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "missing", missing_value_arr);
	}
}

yyjson_mut_val *UpdateNamespacePropertiesResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
