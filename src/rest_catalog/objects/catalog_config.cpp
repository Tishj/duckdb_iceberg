
#include "rest_catalog/objects/catalog_config.hpp"

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

CatalogConfig::CatalogConfig(case_insensitive_map_t<string> defaults_p, case_insensitive_map_t<string> overrides_p,
                             optional<vector<string>> endpoints_p, optional<string> idempotency_key_lifetime_p)
    : defaults(std::move(defaults_p)), overrides(std::move(overrides_p)), endpoints(std::move(endpoints_p)),
      idempotency_key_lifetime(std::move(idempotency_key_lifetime_p)) {
}
CatalogConfig::CatalogConfig(const CatalogConfig &other)
    : defaults(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : other.defaults) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }())),
      overrides(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : other.overrides) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }())),
      endpoints((other.endpoints.has_value() ? optional<vector<string>>(([&]() {
	      vector<string> copied;
	      copied.reserve((*other.endpoints).size());
	      for (const auto &item : (*other.endpoints)) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }()))
                                             : optional<vector<string>>())),
      idempotency_key_lifetime((other.idempotency_key_lifetime.has_value()
                                    ? optional<string>((*other.idempotency_key_lifetime))
                                    : optional<string>())) {
}
CatalogConfig::CatalogConfig(CatalogConfig &&other) : CatalogConfig(static_cast<const CatalogConfig &>(other)) {
}

CatalogConfigBuilder::CatalogConfigBuilder() {
}

CatalogConfigBuilder &CatalogConfigBuilder::SetDefaults(case_insensitive_map_t<string> value) {
	defaults_.emplace(std::move(value));
	has_defaults_ = true;
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetOverrides(case_insensitive_map_t<string> value) {
	overrides_.emplace(std::move(value));
	has_overrides_ = true;
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetEndpoints(vector<string> value) {
	endpoints_.emplace(std::move(value));
	return *this;
}

CatalogConfigBuilder &CatalogConfigBuilder::SetIdempotencyKeyLifetime(string value) {
	idempotency_key_lifetime_.emplace(std::move(value));
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

string CatalogConfig::TryFromJSON(yyjson_val *obj, CatalogConfigBuilder &builder) {
	try {
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
						throw InvalidInputException(StringUtil::Format(
						    "CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
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
						throw InvalidInputException(StringUtil::Format(
						    "CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
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
						throw InvalidInputException(StringUtil::Format(
						    "CatalogConfig property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					endpoints.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("CatalogConfig property 'endpoints' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(endpoints_val)));
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
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CatalogConfig CatalogConfig::FromJSON(yyjson_val *obj) {
	CatalogConfigBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CatalogConfig CatalogConfig::Copy() const {
	return CatalogConfig(*this);
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
