
#include "rest_catalog/objects/register_view_request.hpp"

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

RegisterViewRequest::RegisterViewRequest(string name_p, string metadata_location_p)
    : name(std::move(name_p)), metadata_location(std::move(metadata_location_p)) {
}

RegisterViewRequestBuilder::RegisterViewRequestBuilder() {
}

RegisterViewRequestBuilder &RegisterViewRequestBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

RegisterViewRequestBuilder &RegisterViewRequestBuilder::SetMetadataLocation(string value) {
	metadata_location_.emplace(std::move(value));
	has_metadata_location_ = true;
	return *this;
}

RegisterViewRequest RegisterViewRequestBuilder::Build() {
	if (!has_name_) {
		throw InvalidInputException("RegisterViewRequest required property 'name' is missing");
	}
	if (!has_metadata_location_) {
		throw InvalidInputException("RegisterViewRequest required property 'metadata-location' is missing");
	}
	auto result = RegisterViewRequest(std::move(*name_), std::move(*metadata_location_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RegisterViewRequestBuilder::TryBuild(optional<RegisterViewRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string RegisterViewRequest::TryFromJSON(yyjson_val *obj, RegisterViewRequestBuilder &builder) {
	try {
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("RegisterViewRequest required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RegisterViewRequest property 'name' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
		if (!metadata_location_val) {
			throw InvalidInputException("RegisterViewRequest required property 'metadata-location' is missing");
		} else {
			string metadata_location;
			if (yyjson_is_str(metadata_location_val)) {
				metadata_location = yyjson_get_str(metadata_location_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RegisterViewRequest property 'metadata_location' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(metadata_location_val)));
			}
			builder.SetMetadataLocation(std::move(metadata_location));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RegisterViewRequest RegisterViewRequest::FromJSON(yyjson_val *obj) {
	RegisterViewRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

RegisterViewRequest RegisterViewRequest::Copy() const {
	RegisterViewRequestBuilder builder;
	string name_tmp;
	name_tmp = name;
	builder.SetName(std::move(name_tmp));
	string metadata_location_tmp;
	metadata_location_tmp = metadata_location;
	builder.SetMetadataLocation(std::move(metadata_location_tmp));
	return builder.Build();
}

string RegisterViewRequest::Validate() const {
	string error;
	return "";
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
