
#include "rest_catalog/objects/create_namespace_response.hpp"

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

CreateNamespaceResponse::CreateNamespaceResponse(Namespace _namespace_p,
                                                 optional<case_insensitive_map_t<string>> properties_p)
    : _namespace(std::move(_namespace_p)), properties(std::move(properties_p)) {
}
CreateNamespaceResponse::CreateNamespaceResponse(const CreateNamespaceResponse &other)
    : _namespace(other._namespace.Copy()),
      properties((other.properties.has_value() ? optional<case_insensitive_map_t<string>>(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : (*other.properties)) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }()))
                                               : optional<case_insensitive_map_t<string>>())) {
}
CreateNamespaceResponse::CreateNamespaceResponse(CreateNamespaceResponse &&other)
    : CreateNamespaceResponse(static_cast<const CreateNamespaceResponse &>(other)) {
}

CreateNamespaceResponseBuilder::CreateNamespaceResponseBuilder() {
}

CreateNamespaceResponseBuilder &CreateNamespaceResponseBuilder::SetNamespace(Namespace value) {
	_namespace_.emplace(std::move(value));
	has__namespace_ = true;
	return *this;
}

CreateNamespaceResponseBuilder &CreateNamespaceResponseBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

CreateNamespaceResponse CreateNamespaceResponseBuilder::Build() {
	if (!has__namespace_) {
		throw InvalidInputException("CreateNamespaceResponse required property 'namespace' is missing");
	}
	auto result = CreateNamespaceResponse(std::move(*_namespace_), std::move(properties_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> CreateNamespaceResponseBuilder::TryBuild(optional<CreateNamespaceResponse> &result) {
	if (!has__namespace_) {
		return "CreateNamespaceResponse required property 'namespace' is missing";
	}
	auto built = CreateNamespaceResponse(std::move(*_namespace_), std::move(properties_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> CreateNamespaceResponse::TryFromJSON(yyjson_val *obj, CreateNamespaceResponseBuilder &builder) {
	try {
		auto _namespace_val = yyjson_obj_get(obj, "namespace");
		if (!_namespace_val) {
			throw InvalidInputException("CreateNamespaceResponse required property 'namespace' is missing");
		} else {
			builder.SetNamespace(Namespace::FromJSON(_namespace_val));
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
						    "CreateNamespaceResponse property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("CreateNamespaceResponse property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CreateNamespaceResponse CreateNamespaceResponse::FromJSON(yyjson_val *obj) {
	CreateNamespaceResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

CreateNamespaceResponse CreateNamespaceResponse::Copy() const {
	return CreateNamespaceResponse(*this);
}

optional<string> CreateNamespaceResponse::Validate() const {
	optional<string> error;
	error = _namespace.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void CreateNamespaceResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
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

yyjson_mut_val *CreateNamespaceResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
