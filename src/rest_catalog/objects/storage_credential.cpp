
#include "rest_catalog/objects/storage_credential.hpp"

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

StorageCredential::StorageCredential(string prefix_p, case_insensitive_map_t<string> config_p)
    : prefix(std::move(prefix_p)), config(std::move(config_p)) {
}

StorageCredentialBuilder::StorageCredentialBuilder() {
}

StorageCredentialBuilder &StorageCredentialBuilder::SetPrefix(string value) {
	prefix_.emplace(std::move(value));
	has_prefix_ = true;
	return *this;
}

StorageCredentialBuilder &StorageCredentialBuilder::SetConfig(case_insensitive_map_t<string> value) {
	config_.emplace(std::move(value));
	has_config_ = true;
	return *this;
}

StorageCredential StorageCredentialBuilder::Build() {
	if (!has_prefix_) {
		throw InvalidInputException("StorageCredential required property 'prefix' is missing");
	}
	if (!has_config_) {
		throw InvalidInputException("StorageCredential required property 'config' is missing");
	}
	auto result = StorageCredential(std::move(*prefix_), std::move(*config_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string StorageCredentialBuilder::TryBuild(optional<StorageCredential> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string StorageCredential::TryFromJSON(yyjson_val *obj, StorageCredentialBuilder &builder) {
	try {
		auto prefix_val = yyjson_obj_get(obj, "prefix");
		if (!prefix_val) {
			throw InvalidInputException("StorageCredential required property 'prefix' is missing");
		} else {
			string prefix;
			if (yyjson_is_str(prefix_val)) {
				prefix = yyjson_get_str(prefix_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StorageCredential property 'prefix' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(prefix_val)));
			}
			builder.SetPrefix(std::move(prefix));
		}
		auto config_val = yyjson_obj_get(obj, "config");
		if (!config_val) {
			throw InvalidInputException("StorageCredential required property 'config' is missing");
		} else {
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
						    "StorageCredential property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					config.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("StorageCredential property 'config' is not of type 'object'");
			}
			builder.SetConfig(std::move(config));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

StorageCredential StorageCredential::FromJSON(yyjson_val *obj) {
	StorageCredentialBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

StorageCredential StorageCredential::Copy() const {
	StorageCredentialBuilder builder;
	string prefix_tmp;
	prefix_tmp = prefix;
	builder.SetPrefix(std::move(prefix_tmp));
	case_insensitive_map_t<string> config_tmp;
	for (auto &entry : config) {
		config_tmp.emplace(entry.first, entry.second);
	}
	builder.SetConfig(std::move(config_tmp));
	return builder.Build();
}

string StorageCredential::Validate() const {
	string error;
	return "";
}

void StorageCredential::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: prefix
	yyjson_mut_obj_add_strcpy(doc, obj, "prefix", prefix.c_str());

	// Serialize: config
	yyjson_mut_val *config_obj = yyjson_mut_obj(doc);
	for (const auto &it : config) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, config_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "config", config_obj);
}

yyjson_mut_val *StorageCredential::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
