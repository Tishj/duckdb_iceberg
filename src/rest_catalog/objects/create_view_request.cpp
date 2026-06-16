
#include "rest_catalog/objects/create_view_request.hpp"

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

CreateViewRequest::CreateViewRequest(string name_p, Schema schema_p, ViewVersion view_version_p,
                                     case_insensitive_map_t<string> properties_p, optional<string> location_p)
    : name(std::move(name_p)), schema(std::move(schema_p)), view_version(std::move(view_version_p)),
      properties(std::move(properties_p)), location(std::move(location_p)) {
}

CreateViewRequestBuilder::CreateViewRequestBuilder() {
}

CreateViewRequestBuilder &CreateViewRequestBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

CreateViewRequestBuilder &CreateViewRequestBuilder::SetSchema(Schema value) {
	schema_.emplace(std::move(value));
	has_schema_ = true;
	return *this;
}

CreateViewRequestBuilder &CreateViewRequestBuilder::SetViewVersion(ViewVersion value) {
	view_version_.emplace(std::move(value));
	has_view_version_ = true;
	return *this;
}

CreateViewRequestBuilder &CreateViewRequestBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	has_properties_ = true;
	return *this;
}

CreateViewRequestBuilder &CreateViewRequestBuilder::SetLocation(string value) {
	location_.emplace(std::move(value));
	return *this;
}

CreateViewRequest CreateViewRequestBuilder::Build() {
	if (!has_name_) {
		throw InvalidInputException("CreateViewRequest required property 'name' is missing");
	}
	if (!has_schema_) {
		throw InvalidInputException("CreateViewRequest required property 'schema' is missing");
	}
	if (!has_view_version_) {
		throw InvalidInputException("CreateViewRequest required property 'view-version' is missing");
	}
	if (!has_properties_) {
		throw InvalidInputException("CreateViewRequest required property 'properties' is missing");
	}
	auto result = CreateViewRequest(std::move(*name_), std::move(*schema_), std::move(*view_version_),
	                                std::move(*properties_), std::move(location_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CreateViewRequestBuilder::TryBuild(optional<CreateViewRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CreateViewRequest::TryFromJSON(yyjson_val *obj, CreateViewRequestBuilder &builder) {
	try {
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("CreateViewRequest required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("CreateViewRequest property 'name' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		auto schema_val = yyjson_obj_get(obj, "schema");
		if (!schema_val) {
			throw InvalidInputException("CreateViewRequest required property 'schema' is missing");
		} else {
			builder.SetSchema(Schema::FromJSON(schema_val));
		}
		auto view_version_val = yyjson_obj_get(obj, "view-version");
		if (!view_version_val) {
			throw InvalidInputException("CreateViewRequest required property 'view-version' is missing");
		} else {
			builder.SetViewVersion(ViewVersion::FromJSON(view_version_val));
		}
		auto properties_val = yyjson_obj_get(obj, "properties");
		if (!properties_val) {
			throw InvalidInputException("CreateViewRequest required property 'properties' is missing");
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
						    "CreateViewRequest property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("CreateViewRequest property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		auto location_val = yyjson_obj_get(obj, "location");
		if (location_val) {
			string location;
			if (yyjson_is_str(location_val)) {
				location = yyjson_get_str(location_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "CreateViewRequest property 'location' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(location_val)));
			}
			builder.SetLocation(std::move(location));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CreateViewRequest CreateViewRequest::FromJSON(yyjson_val *obj) {
	CreateViewRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CreateViewRequest CreateViewRequest::Copy() const {
	CreateViewRequestBuilder builder;
	string name_tmp;
	name_tmp = name;
	builder.SetName(std::move(name_tmp));
	auto schema_tmp = schema.Copy();
	builder.SetSchema(std::move(schema_tmp));
	auto view_version_tmp = view_version.Copy();
	builder.SetViewVersion(std::move(view_version_tmp));
	case_insensitive_map_t<string> properties_tmp;
	for (auto &entry : properties) {
		properties_tmp.emplace(entry.first, entry.second);
	}
	builder.SetProperties(std::move(properties_tmp));
	optional<string> location_tmp;
	if (location.has_value()) {
		location_tmp.emplace();
		(*location_tmp) = (*location);
	}
	if (location_tmp.has_value()) {
		builder.SetLocation(std::move((*location_tmp)));
	}
	return builder.Build();
}

string CreateViewRequest::Validate() const {
	string error;
	error = schema.Validate();
	if (!error.empty()) {
		return error;
	}
	error = view_version.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void CreateViewRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: schema
	yyjson_mut_val *schema_val = schema.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "schema", schema_val);

	// Serialize: view-version
	yyjson_mut_val *view_version_val = view_version.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "view-version", view_version_val);

	// Serialize: properties
	yyjson_mut_val *properties_obj = yyjson_mut_obj(doc);
	for (const auto &it : properties) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, properties_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "properties", properties_obj);

	// Serialize: location
	if (location.has_value()) {
		auto &location_value = *location;
		yyjson_mut_obj_add_strcpy(doc, obj, "location", location_value.c_str());
	}
}

yyjson_mut_val *CreateViewRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
