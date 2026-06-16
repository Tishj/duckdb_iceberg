
#include "rest_catalog/objects/load_table_result.hpp"

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

LoadTableResult::LoadTableResult(TableMetadata metadata_p, optional<string> metadata_location_p,
                                 optional<case_insensitive_map_t<string>> config_p,
                                 optional<vector<StorageCredential>> storage_credentials_p)
    : metadata(std::move(metadata_p)), metadata_location(std::move(metadata_location_p)), config(std::move(config_p)),
      storage_credentials(std::move(storage_credentials_p)) {
}
LoadTableResult::LoadTableResult(const LoadTableResult &other)
    : metadata(other.metadata.Copy()),
      metadata_location(
          (other.metadata_location.has_value() ? optional<string>((*other.metadata_location)) : optional<string>())),
      config((other.config.has_value() ? optional<case_insensitive_map_t<string>>(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : (*other.config)) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }()))
                                       : optional<case_insensitive_map_t<string>>())),
      storage_credentials((other.storage_credentials.has_value() ? optional<vector<StorageCredential>>(([&]() {
	      vector<StorageCredential> copied;
	      copied.reserve((*other.storage_credentials).size());
	      for (const auto &item : (*other.storage_credentials)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                                 : optional<vector<StorageCredential>>())) {
}
LoadTableResult::LoadTableResult(LoadTableResult &&other)
    : LoadTableResult(static_cast<const LoadTableResult &>(other)) {
}

LoadTableResultBuilder::LoadTableResultBuilder() {
}

LoadTableResultBuilder &LoadTableResultBuilder::SetMetadata(TableMetadata value) {
	metadata_.emplace(std::move(value));
	has_metadata_ = true;
	return *this;
}

LoadTableResultBuilder &LoadTableResultBuilder::SetMetadataLocation(string value) {
	metadata_location_.emplace(std::move(value));
	return *this;
}

LoadTableResultBuilder &LoadTableResultBuilder::SetConfig(case_insensitive_map_t<string> value) {
	config_.emplace(std::move(value));
	return *this;
}

LoadTableResultBuilder &LoadTableResultBuilder::SetStorageCredentials(vector<StorageCredential> value) {
	storage_credentials_.emplace(std::move(value));
	return *this;
}

LoadTableResult LoadTableResultBuilder::Build() {
	if (!has_metadata_) {
		throw InvalidInputException("LoadTableResult required property 'metadata' is missing");
	}
	auto result = LoadTableResult(std::move(*metadata_), std::move(metadata_location_), std::move(config_),
	                              std::move(storage_credentials_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string LoadTableResultBuilder::TryBuild(optional<LoadTableResult> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string LoadTableResult::TryFromJSON(yyjson_val *obj, LoadTableResultBuilder &builder) {
	try {
		auto metadata_val = yyjson_obj_get(obj, "metadata");
		if (!metadata_val) {
			throw InvalidInputException("LoadTableResult required property 'metadata' is missing");
		} else {
			builder.SetMetadata(TableMetadata::FromJSON(metadata_val));
		}
		auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
		if (metadata_location_val) {
			if (yyjson_is_null(metadata_location_val)) {
				//! do nothing, property is explicitly nullable
			} else {
				string metadata_location;
				if (yyjson_is_str(metadata_location_val)) {
					metadata_location = yyjson_get_str(metadata_location_val);
				} else {
					throw InvalidInputException(StringUtil::Format(
					    "LoadTableResult property 'metadata_location' is not of type 'string', found '%s' instead",
					    yyjson_get_type_desc(metadata_location_val)));
				}
				builder.SetMetadataLocation(std::move(metadata_location));
			}
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
						throw InvalidInputException(StringUtil::Format(
						    "LoadTableResult property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					config.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("LoadTableResult property 'config' is not of type 'object'");
			}
			builder.SetConfig(std::move(config));
		}
		auto storage_credentials_val = yyjson_obj_get(obj, "storage-credentials");
		if (storage_credentials_val) {
			vector<StorageCredential> storage_credentials;
			if (yyjson_is_arr(storage_credentials_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(storage_credentials_val, idx, max, val) {
					auto tmp = StorageCredential::FromJSON(val);
					storage_credentials.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "LoadTableResult property 'storage_credentials' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(storage_credentials_val)));
			}
			builder.SetStorageCredentials(std::move(storage_credentials));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LoadTableResult LoadTableResult::FromJSON(yyjson_val *obj) {
	LoadTableResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

LoadTableResult LoadTableResult::Copy() const {
	return LoadTableResult(*this);
}

string LoadTableResult::Validate() const {
	string error;
	error = metadata.Validate();
	if (!error.empty()) {
		return error;
	}
	if (storage_credentials.has_value()) {
		for (const auto &item : (*storage_credentials)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void LoadTableResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: metadata
	yyjson_mut_val *metadata_val = metadata.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "metadata", metadata_val);

	// Serialize: metadata-location
	if (metadata_location.has_value()) {
		auto &metadata_location_value = *metadata_location;
		yyjson_mut_obj_add_strcpy(doc, obj, "metadata-location", metadata_location_value.c_str());
	}

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

	// Serialize: storage-credentials
	if (storage_credentials.has_value()) {
		auto &storage_credentials_value = *storage_credentials;
		yyjson_mut_val *storage_credentials_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : storage_credentials_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(storage_credentials_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "storage-credentials", storage_credentials_value_arr);
	}
}

yyjson_mut_val *LoadTableResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
