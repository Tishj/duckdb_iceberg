
#include "rest_catalog/objects/load_view_result.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

LoadViewResult::LoadViewResult(string metadata_location_p, ViewMetadata metadata_p,
                               optional<case_insensitive_map_t<string>> config_p)
    : metadata_location(std::move(metadata_location_p)), metadata(std::move(metadata_p)), config(std::move(config_p)) {
}

LoadViewResultBuilder::LoadViewResultBuilder() {
}

LoadViewResultBuilder &LoadViewResultBuilder::SetMetadataLocation(string value) {
	metadata_location_ = std::move(value);
	has_metadata_location_ = true;
	return *this;
}

LoadViewResultBuilder &LoadViewResultBuilder::SetMetadata(ViewMetadata value) {
	metadata_ = std::move(value);
	has_metadata_ = true;
	return *this;
}

LoadViewResultBuilder &LoadViewResultBuilder::SetConfig(case_insensitive_map_t<string> value) {
	config_ = std::move(value);
	return *this;
}

LoadViewResult LoadViewResultBuilder::Build() {
	if (!has_metadata_location_) {
		throw InvalidInputException("LoadViewResult required property 'metadata-location' is missing");
	}
	if (!has_metadata_) {
		throw InvalidInputException("LoadViewResult required property 'metadata' is missing");
	}
	auto result = LoadViewResult(std::move(*metadata_location_), std::move(*metadata_), std::move(config_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string LoadViewResultBuilder::TryBuild(optional<LoadViewResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LoadViewResult LoadViewResult::FromJSON(yyjson_val *obj) {
	LoadViewResultBuilder builder;
	auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
	if (!metadata_location_val) {
		throw InvalidInputException("LoadViewResult required property 'metadata-location' is missing");
	} else {
		string metadata_location;
		if (yyjson_is_str(metadata_location_val)) {
			metadata_location = yyjson_get_str(metadata_location_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "LoadViewResult property 'metadata_location' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(metadata_location_val)));
		}
		builder.SetMetadataLocation(std::move(metadata_location));
	}
	auto metadata_val = yyjson_obj_get(obj, "metadata");
	if (!metadata_val) {
		throw InvalidInputException("LoadViewResult required property 'metadata' is missing");
	} else {
		optional<ViewMetadata> metadata;
		metadata = ViewMetadata::FromJSON(metadata_val);
		builder.SetMetadata(std::move(*metadata));
	}
	auto config_val = yyjson_obj_get(obj, "config");
	if (config_val) {
		case_insensitive_map_t<string> config;
		if (yyjson_is_obj(config_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(config_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("LoadViewResult property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				config.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("LoadViewResult property 'config' is not of type 'object'");
		}
		builder.SetConfig(std::move(config));
	}
	return builder.Build();
}

string LoadViewResult::TryFromJSON(yyjson_val *obj, optional<LoadViewResult> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LoadViewResult LoadViewResult::Copy() const {
	LoadViewResultBuilder builder;
	string metadata_location_tmp;
	metadata_location_tmp = metadata_location;
	builder.SetMetadataLocation(std::move(metadata_location_tmp));
	optional<ViewMetadata> metadata_tmp;
	metadata_tmp = metadata.Copy();
	builder.SetMetadata(std::move(*metadata_tmp));
	case_insensitive_map_t<string> config_tmp;
	if (config.has_value()) {
		config_tmp.emplace();
		for (auto &entry : (*config)) {
			(*config_tmp).emplace(entry.first, entry.second);
		}
	}
	if (config_tmp.has_value()) {
		builder.SetConfig(std::move(config_tmp));
	}
	return builder.Build();
}

string LoadViewResult::Validate() const {
	string error;
	error = metadata.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void LoadViewResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: metadata-location
	yyjson_mut_obj_add_strcpy(doc, obj, "metadata-location", metadata_location.c_str());

	// Serialize: metadata
	yyjson_mut_val *metadata_val = metadata.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "metadata", metadata_val);

	// Serialize: config
	if (config.has_value()) {
		auto &config_value = *config;
		yyjson_mut_val *config_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : config_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, config_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "config", config_value_obj);
	}
}

yyjson_mut_val *LoadViewResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
