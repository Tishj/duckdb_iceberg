
#include "rest_catalog/objects/catalog_config.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

CatalogConfig::CatalogConfig(case_insensitive_map_t<string> defaults_p, case_insensitive_map_t<string> overrides_p,
                             optional<vector<string>> endpoints_p, optional<string> idempotency_key_lifetime_p)
    : defaults(std::move(defaults_p)), overrides(std::move(overrides_p)), endpoints(std::move(endpoints_p)),
      idempotency_key_lifetime(std::move(idempotency_key_lifetime_p)) {
}

CatalogConfigBuilder::CatalogConfigBuilder() {
}

CatalogConfigBuilder &CatalogConfigBuilder::SetDefaults(case_insensitive_map_t<string> value) {
	defaults_ = std::move(value);
	has_defaults_ = true;
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetOverrides(case_insensitive_map_t<string> value) {
	overrides_ = std::move(value);
	has_overrides_ = true;
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetEndpoints(vector<string> value) {
	endpoints_ = std::move(value);
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetIdempotencyKeyLifetime(string value) {
	idempotency_key_lifetime_ = std::move(value);
	return *this;
}

CatalogConfig CatalogConfigBuilder::Build() {
	if (!has_defaults_) {
		throw InvalidInputException("CatalogConfig required property 'defaults' is missing");
	}
	if (!has_overrides_) {
		throw InvalidInputException("CatalogConfig required property 'overrides' is missing");
	}
	auto result = CatalogConfig(std::move(*defaults_), std::move(*overrides_), std::move(endpoints_),
	                            std::move(idempotency_key_lifetime_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CatalogConfigBuilder::TryBuild(optional<CatalogConfig> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CatalogConfig CatalogConfig::FromJSON(yyjson_val *obj) {
	CatalogConfigBuilder builder;
	auto defaults_val = yyjson_obj_get(obj, "defaults");
	if (!defaults_val) {
		throw InvalidInputException("CatalogConfig required property 'defaults' is missing");
	} else {
		case_insensitive_map_t<string> defaults;
		if (yyjson_is_obj(defaults_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(defaults_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				defaults.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("CatalogConfig property 'defaults' is not of type 'object'");
		}
		builder.SetDefaults(std::move(defaults));
	}
	auto overrides_val = yyjson_obj_get(obj, "overrides");
	if (!overrides_val) {
		throw InvalidInputException("CatalogConfig required property 'overrides' is missing");
	} else {
		case_insensitive_map_t<string> overrides;
		if (yyjson_is_obj(overrides_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(overrides_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				overrides.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("CatalogConfig property 'overrides' is not of type 'object'");
		}
		builder.SetOverrides(std::move(overrides));
	}
	auto endpoints_val = yyjson_obj_get(obj, "endpoints");
	if (endpoints_val) {
		vector<string> endpoints;
		if (yyjson_is_arr(endpoints_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(endpoints_val, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				endpoints.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("CatalogConfig property 'endpoints' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(endpoints_val));
		}
		builder.SetEndpoints(std::move(endpoints));
	}
	auto idempotency_key_lifetime_val = yyjson_obj_get(obj, "idempotency-key-lifetime");
	if (idempotency_key_lifetime_val) {
		string idempotency_key_lifetime;
		if (yyjson_is_str(idempotency_key_lifetime_val)) {
			idempotency_key_lifetime = yyjson_get_str(idempotency_key_lifetime_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "CatalogConfig property 'idempotency_key_lifetime' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(idempotency_key_lifetime_val)));
		}
		builder.SetIdempotencyKeyLifetime(std::move(idempotency_key_lifetime));
	}
	return builder.Build();
}

string CatalogConfig::TryFromJSON(yyjson_val *obj, optional<CatalogConfig> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CatalogConfig CatalogConfig::Copy() const {
	CatalogConfigBuilder builder;
	case_insensitive_map_t<string> defaults_tmp;
	for (auto &entry : defaults) {
		defaults_tmp.emplace(entry.first, entry.second);
	}
	builder.SetDefaults(std::move(defaults_tmp));
	case_insensitive_map_t<string> overrides_tmp;
	for (auto &entry : overrides) {
		overrides_tmp.emplace(entry.first, entry.second);
	}
	builder.SetOverrides(std::move(overrides_tmp));
	vector<string> endpoints_tmp;
	if (endpoints.has_value()) {
		endpoints_tmp.emplace();
		(*endpoints_tmp).reserve((*endpoints).size());
		for (auto &item : (*endpoints)) {
			(*endpoints_tmp).emplace_back(item);
		}
	}
	if (endpoints_tmp.has_value()) {
		builder.SetEndpoints(std::move(endpoints_tmp));
	}
	string idempotency_key_lifetime_tmp;
	if (idempotency_key_lifetime.has_value()) {
		idempotency_key_lifetime_tmp.emplace();
		(*idempotency_key_lifetime_tmp) = (*idempotency_key_lifetime);
	}
	if (idempotency_key_lifetime_tmp.has_value()) {
		builder.SetIdempotencyKeyLifetime(std::move(idempotency_key_lifetime_tmp));
	}
	return builder.Build();
}

string CatalogConfig::Validate() const {
	string error;
	return "";
}

void CatalogConfig::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: defaults
	yyjson_mut_val *defaults_obj = yyjson_mut_obj(doc);
	for (const auto &it : defaults) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, defaults_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "defaults", defaults_obj);

	// Serialize: overrides
	yyjson_mut_val *overrides_obj = yyjson_mut_obj(doc);
	for (const auto &it : overrides) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, overrides_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "overrides", overrides_obj);

	// Serialize: endpoints
	if (endpoints.has_value()) {
		auto &endpoints_value = *endpoints;
		yyjson_mut_val *endpoints_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : endpoints_value) {
			yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
			yyjson_mut_arr_append(endpoints_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "endpoints", endpoints_value_arr);
	}

	// Serialize: idempotency-key-lifetime
	if (idempotency_key_lifetime.has_value()) {
		auto &idempotency_key_lifetime_value = *idempotency_key_lifetime;
		yyjson_mut_obj_add_strcpy(doc, obj, "idempotency-key-lifetime", idempotency_key_lifetime_value.c_str());
	}
}

yyjson_mut_val *CatalogConfig::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
