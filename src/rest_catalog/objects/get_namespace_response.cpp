
#include "rest_catalog/objects/get_namespace_response.hpp"

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

GetNamespaceResponse::GetNamespaceResponse(Namespace _namespace_p,
                                           optional<case_insensitive_map_t<string>> properties_p)
    : _namespace(std::move(_namespace_p)), properties(std::move(properties_p)) {
}

GetNamespaceResponseBuilder::GetNamespaceResponseBuilder() {
}

GetNamespaceResponseBuilder &GetNamespaceResponseBuilder::SetNamespace(Namespace value) {
	_namespace_.emplace(std::move(value));
	has__namespace_ = true;
	return *this;
}

GetNamespaceResponseBuilder &GetNamespaceResponseBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

GetNamespaceResponse GetNamespaceResponseBuilder::Build() {
	if (!has__namespace_) {
		throw InvalidInputException("GetNamespaceResponse required property 'namespace' is missing");
	}
	auto result = GetNamespaceResponse(std::move(*_namespace_), std::move(properties_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string GetNamespaceResponseBuilder::TryBuild(optional<GetNamespaceResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string GetNamespaceResponse::TryFromJSON(yyjson_val *obj, GetNamespaceResponseBuilder &builder) {
	try {
		auto _namespace_val = yyjson_obj_get(obj, "namespace");
		if (!_namespace_val) {
			throw InvalidInputException("GetNamespaceResponse required property 'namespace' is missing");
		} else {
			builder.SetNamespace(Namespace::FromJSON(_namespace_val));
		}
		auto properties_val = yyjson_obj_get(obj, "properties");
		if (properties_val) {
			if (yyjson_is_null(properties_val)) {
				//! do nothing, property is explicitly nullable
			} else {
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
							    "GetNamespaceResponse property 'tmp' is not of type 'string', found '%s' instead",
							    yyjson_get_type_desc(val)));
						}
						properties.emplace(key_str, std::move(tmp));
					}
				} else {
					throw InvalidInputException("GetNamespaceResponse property 'properties' is not of type 'object'");
				}
				builder.SetProperties(std::move(properties));
			}
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

GetNamespaceResponse GetNamespaceResponse::FromJSON(yyjson_val *obj) {
	GetNamespaceResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

GetNamespaceResponse GetNamespaceResponse::Copy() const {
	GetNamespaceResponseBuilder builder;
	auto _namespace_tmp = _namespace.Copy();
	builder.SetNamespace(std::move(_namespace_tmp));
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

string GetNamespaceResponse::Validate() const {
	string error;
	error = _namespace.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void GetNamespaceResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: namespace
	yyjson_mut_val *_namespace_val = _namespace.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "namespace", _namespace_val);

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

yyjson_mut_val *GetNamespaceResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
