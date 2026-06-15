
#include "rest_catalog/objects/register_view_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RegisterViewRequest::RegisterViewRequest() {
}

RegisterViewRequestBuilder::RegisterViewRequestBuilder() {
}

RegisterViewRequestBuilder &RegisterViewRequestBuilder::SetName(string value) {
	result_.name = std::move(value);
	has_name_ = true;
	return *this;
}

RegisterViewRequestBuilder &RegisterViewRequestBuilder::SetMetadataLocation(string value) {
	result_.metadata_location = std::move(value);
	has_metadata_location_ = true;
	return *this;
}

string RegisterViewRequestBuilder::TryBuild(RegisterViewRequest &result) {
	if (!has_name_) {
		return "RegisterViewRequest required property 'name' is missing";
	}
	if (!has_metadata_location_) {
		return "RegisterViewRequest required property 'metadata-location' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RegisterViewRequest RegisterViewRequestBuilder::Build() {
	RegisterViewRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RegisterViewRequest RegisterViewRequest::FromJSON(yyjson_val *obj) {
	RegisterViewRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RegisterViewRequest RegisterViewRequest::Copy() const {
	RegisterViewRequest res;
	res.name = name;
	res.metadata_location = metadata_location;
	return res;
}

string RegisterViewRequest::Validate() const {
	string error;
	return "";
}

string RegisterViewRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	auto name_val = yyjson_obj_get(obj, "name");
	if (!name_val) {
		return "RegisterViewRequest required property 'name' is missing";
	} else {
		if (yyjson_is_str(name_val)) {
			name = yyjson_get_str(name_val);
		} else {
			return StringUtil::Format("RegisterViewRequest property 'name' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(name_val));
		}
	}
	auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
	if (!metadata_location_val) {
		return "RegisterViewRequest required property 'metadata-location' is missing";
	} else {
		if (yyjson_is_str(metadata_location_val)) {
			metadata_location = yyjson_get_str(metadata_location_val);
		} else {
			return StringUtil::Format(
			    "RegisterViewRequest property 'metadata_location' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(metadata_location_val));
		}
	}
	return Validate();
}

void RegisterViewRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: metadata-location
	yyjson_mut_obj_add_strcpy(doc, obj, "metadata-location", metadata_location.c_str());
}

yyjson_mut_val *RegisterViewRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
