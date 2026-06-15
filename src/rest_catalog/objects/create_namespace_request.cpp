
#include "rest_catalog/objects/create_namespace_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

CreateNamespaceRequest::CreateNamespaceRequest()
    : properties(GeneratedObjectAccess::Create<optional<case_insensitive_map_t<string>>>()) {
}

CreateNamespaceRequestBuilder::CreateNamespaceRequestBuilder() {
}

CreateNamespaceRequestBuilder &CreateNamespaceRequestBuilder::SetNamespace(Namespace value) {
	result_._namespace = std::move(value);
	has__namespace_ = true;
	return *this;
}

CreateNamespaceRequestBuilder &CreateNamespaceRequestBuilder::SetProperties(case_insensitive_map_t<string> value) {
	result_.properties = std::move(value);
	return *this;
}

string CreateNamespaceRequestBuilder::TryBuild(CreateNamespaceRequest &result) {
	if (!has__namespace_) {
		return "CreateNamespaceRequest required property 'namespace' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

CreateNamespaceRequest CreateNamespaceRequestBuilder::Build() {
	CreateNamespaceRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

CreateNamespaceRequest CreateNamespaceRequest::FromJSON(yyjson_val *obj) {
	CreateNamespaceRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

CreateNamespaceRequest CreateNamespaceRequest::Copy() const {
	CreateNamespaceRequest res;
	res._namespace = _namespace.Copy();
	if (properties.has_value()) {
		res.properties = GeneratedObjectAccess::Create<case_insensitive_map_t<string>>();
		for (auto &entry : (*properties)) {
			(*res.properties).emplace(entry.first, entry.second);
		}
	}
	return res;
}

string CreateNamespaceRequest::Validate() const {
	string error;
	error = _namespace.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string CreateNamespaceRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	auto _namespace_val = yyjson_obj_get(obj, "namespace");
	if (!_namespace_val) {
		return "CreateNamespaceRequest required property 'namespace' is missing";
	} else {
		error = _namespace.TryFromJSON(_namespace_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto properties_val = yyjson_obj_get(obj, "properties");
	if (properties_val) {
		case_insensitive_map_t<string> properties_tmp;
		if (yyjson_is_obj(properties_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(properties_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					return StringUtil::Format(
					    "CreateNamespaceRequest property 'tmp' is not of type 'string', found '%s' instead",
					    yyjson_get_type_desc(val));
				}
				properties_tmp.emplace(key_str, std::move(tmp));
			}
		} else {
			return "CreateNamespaceRequest property 'properties_tmp' is not of type 'object'";
		}
		properties = std::move(properties_tmp);
	}
	return Validate();
}

void CreateNamespaceRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
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

yyjson_mut_val *CreateNamespaceRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
