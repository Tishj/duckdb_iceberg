
#include "rest_catalog/objects/view_metadata.hpp"

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

ViewMetadata::ViewMetadata(string view_uuid_p, int32_t format_version_p, string location_p,
                           int32_t current_version_id_p, vector<ViewVersion> versions_p,
                           vector<ViewHistoryEntry> version_log_p, vector<Schema> schemas_p,
                           optional<case_insensitive_map_t<string>> properties_p)
    : view_uuid(std::move(view_uuid_p)), format_version(std::move(format_version_p)), location(std::move(location_p)),
      current_version_id(std::move(current_version_id_p)), versions(std::move(versions_p)),
      version_log(std::move(version_log_p)), schemas(std::move(schemas_p)), properties(std::move(properties_p)) {
}

ViewMetadataBuilder::ViewMetadataBuilder() {
}

ViewMetadataBuilder &ViewMetadataBuilder::SetViewUuid(string value) {
	view_uuid_.emplace(std::move(value));
	has_view_uuid_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetFormatVersion(int32_t value) {
	format_version_.emplace(std::move(value));
	has_format_version_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetLocation(string value) {
	location_.emplace(std::move(value));
	has_location_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetCurrentVersionId(int32_t value) {
	current_version_id_.emplace(std::move(value));
	has_current_version_id_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetVersions(vector<ViewVersion> value) {
	versions_.emplace(std::move(value));
	has_versions_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetVersionLog(vector<ViewHistoryEntry> value) {
	version_log_.emplace(std::move(value));
	has_version_log_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetSchemas(vector<Schema> value) {
	schemas_.emplace(std::move(value));
	has_schemas_ = true;
	return *this;
}

ViewMetadataBuilder &ViewMetadataBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

ViewMetadata ViewMetadataBuilder::Build() {
	if (!has_view_uuid_) {
		throw InvalidInputException("ViewMetadata required property 'view-uuid' is missing");
	}
	if (!has_format_version_) {
		throw InvalidInputException("ViewMetadata required property 'format-version' is missing");
	}
	if (!has_location_) {
		throw InvalidInputException("ViewMetadata required property 'location' is missing");
	}
	if (!has_current_version_id_) {
		throw InvalidInputException("ViewMetadata required property 'current-version-id' is missing");
	}
	if (!has_versions_) {
		throw InvalidInputException("ViewMetadata required property 'versions' is missing");
	}
	if (!has_version_log_) {
		throw InvalidInputException("ViewMetadata required property 'version-log' is missing");
	}
	if (!has_schemas_) {
		throw InvalidInputException("ViewMetadata required property 'schemas' is missing");
	}
	auto result = ViewMetadata(std::move(*view_uuid_), std::move(*format_version_), std::move(*location_),
	                           std::move(*current_version_id_), std::move(*versions_), std::move(*version_log_),
	                           std::move(*schemas_), std::move(properties_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewMetadataBuilder::TryBuild(optional<ViewMetadata> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string ViewMetadata::TryFromJSON(yyjson_val *obj, ViewMetadataBuilder &builder) {
	try {
		auto view_uuid_val = yyjson_obj_get(obj, "view-uuid");
		if (!view_uuid_val) {
			throw InvalidInputException("ViewMetadata required property 'view-uuid' is missing");
		} else {
			string view_uuid;
			if (yyjson_is_str(view_uuid_val)) {
				view_uuid = yyjson_get_str(view_uuid_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ViewMetadata property 'view_uuid' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(view_uuid_val)));
			}
			builder.SetViewUuid(std::move(view_uuid));
		}
		auto format_version_val = yyjson_obj_get(obj, "format-version");
		if (!format_version_val) {
			throw InvalidInputException("ViewMetadata required property 'format-version' is missing");
		} else {
			int32_t format_version;
			if (yyjson_is_int(format_version_val)) {
				format_version = yyjson_get_int(format_version_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ViewMetadata property 'format_version' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(format_version_val)));
			}
			builder.SetFormatVersion(std::move(format_version));
		}
		auto location_val = yyjson_obj_get(obj, "location");
		if (!location_val) {
			throw InvalidInputException("ViewMetadata required property 'location' is missing");
		} else {
			string location;
			if (yyjson_is_str(location_val)) {
				location = yyjson_get_str(location_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ViewMetadata property 'location' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(location_val)));
			}
			builder.SetLocation(std::move(location));
		}
		auto current_version_id_val = yyjson_obj_get(obj, "current-version-id");
		if (!current_version_id_val) {
			throw InvalidInputException("ViewMetadata required property 'current-version-id' is missing");
		} else {
			int32_t current_version_id;
			if (yyjson_is_int(current_version_id_val)) {
				current_version_id = yyjson_get_int(current_version_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ViewMetadata property 'current_version_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(current_version_id_val)));
			}
			builder.SetCurrentVersionId(std::move(current_version_id));
		}
		auto versions_val = yyjson_obj_get(obj, "versions");
		if (!versions_val) {
			throw InvalidInputException("ViewMetadata required property 'versions' is missing");
		} else {
			vector<ViewVersion> versions;
			if (yyjson_is_arr(versions_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(versions_val, idx, max, val) {
					auto tmp = ViewVersion::FromJSON(val);
					versions.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ViewMetadata property 'versions' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(versions_val)));
			}
			builder.SetVersions(std::move(versions));
		}
		auto version_log_val = yyjson_obj_get(obj, "version-log");
		if (!version_log_val) {
			throw InvalidInputException("ViewMetadata required property 'version-log' is missing");
		} else {
			vector<ViewHistoryEntry> version_log;
			if (yyjson_is_arr(version_log_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(version_log_val, idx, max, val) {
					auto tmp = ViewHistoryEntry::FromJSON(val);
					version_log.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ViewMetadata property 'version_log' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(version_log_val)));
			}
			builder.SetVersionLog(std::move(version_log));
		}
		auto schemas_val = yyjson_obj_get(obj, "schemas");
		if (!schemas_val) {
			throw InvalidInputException("ViewMetadata required property 'schemas' is missing");
		} else {
			vector<Schema> schemas;
			if (yyjson_is_arr(schemas_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(schemas_val, idx, max, val) {
					auto tmp = Schema::FromJSON(val);
					schemas.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ViewMetadata property 'schemas' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(schemas_val)));
			}
			builder.SetSchemas(std::move(schemas));
		}
		auto properties_val = yyjson_obj_get(obj, "properties");
		if (properties_val) {
			case_insensitive_map_t<string> properties;
			if (yyjson_is_obj(properties_val)) {
				size_t idx, max;
				yyjson_val *key, *val;
				yyjson_obj_foreach(properties_val, idx, max, key, val) {
					auto key_str = yyjson_get_str(key);
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "ViewMetadata property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("ViewMetadata property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewMetadata ViewMetadata::FromJSON(yyjson_val *obj) {
	ViewMetadataBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

ViewMetadata ViewMetadata::Copy() const {
	ViewMetadataBuilder builder;
	string view_uuid_tmp;
	view_uuid_tmp = view_uuid;
	builder.SetViewUuid(std::move(view_uuid_tmp));
	int32_t format_version_tmp;
	format_version_tmp = format_version;
	builder.SetFormatVersion(std::move(format_version_tmp));
	string location_tmp;
	location_tmp = location;
	builder.SetLocation(std::move(location_tmp));
	int32_t current_version_id_tmp;
	current_version_id_tmp = current_version_id;
	builder.SetCurrentVersionId(std::move(current_version_id_tmp));
	vector<ViewVersion> versions_tmp;
	versions_tmp.reserve(versions.size());
	for (auto &item : versions) {
		versions_tmp.emplace_back(item.Copy());
	}
	builder.SetVersions(std::move(versions_tmp));
	vector<ViewHistoryEntry> version_log_tmp;
	version_log_tmp.reserve(version_log.size());
	for (auto &item : version_log) {
		version_log_tmp.emplace_back(item.Copy());
	}
	builder.SetVersionLog(std::move(version_log_tmp));
	vector<Schema> schemas_tmp;
	schemas_tmp.reserve(schemas.size());
	for (auto &item : schemas) {
		schemas_tmp.emplace_back(item.Copy());
	}
	builder.SetSchemas(std::move(schemas_tmp));
	optional<case_insensitive_map_t<string>> properties_tmp;
	if (properties.has_value()) {
		properties_tmp.emplace();
		for (auto &entry : (*properties)) {
			(*properties_tmp).emplace(entry.first, entry.second);
		}
	}
	if (properties_tmp.has_value()) {
		builder.SetProperties(std::move((*properties_tmp)));
	}
	return builder.Build();
}

string ViewMetadata::Validate() const {
	string error;
	if (format_version < 1) {
		return "ViewMetadata property 'format-version' must be at least 1";
	}
	if (format_version > 1) {
		return "ViewMetadata property 'format-version' must be at most 1";
	}
	for (const auto &item : versions) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	for (const auto &item : version_log) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	for (const auto &item : schemas) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void ViewMetadata::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: view-uuid
	yyjson_mut_obj_add_strcpy(doc, obj, "view-uuid", view_uuid.c_str());

	// Serialize: format-version
	yyjson_mut_obj_add_int(doc, obj, "format-version", format_version);

	// Serialize: location
	yyjson_mut_obj_add_strcpy(doc, obj, "location", location.c_str());

	// Serialize: current-version-id
	yyjson_mut_obj_add_int(doc, obj, "current-version-id", current_version_id);

	// Serialize: versions
	yyjson_mut_val *versions_arr = yyjson_mut_arr(doc);
	for (const auto &item : versions) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(versions_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "versions", versions_arr);

	// Serialize: version-log
	yyjson_mut_val *version_log_arr = yyjson_mut_arr(doc);
	for (const auto &item : version_log) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(version_log_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "version-log", version_log_arr);

	// Serialize: schemas
	yyjson_mut_val *schemas_arr = yyjson_mut_arr(doc);
	for (const auto &item : schemas) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(schemas_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "schemas", schemas_arr);

	// Serialize: properties
	if (properties.has_value()) {
		auto &properties_value = *properties;
		yyjson_mut_val *properties_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : properties_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, properties_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "properties", properties_value_obj);
	}
}

yyjson_mut_val *ViewMetadata::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
