
#include "rest_catalog/objects/register_table_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RegisterTableRequest::RegisterTableRequest() {
}

RegisterTableRequestBuilder::RegisterTableRequestBuilder() {
}

RegisterTableRequestBuilder &RegisterTableRequestBuilder::SetName(string value) {
	result_.name = std::move(value);
	has_name_ = true;
	return *this;
}

RegisterTableRequestBuilder &RegisterTableRequestBuilder::SetMetadataLocation(string value) {
	result_.metadata_location = std::move(value);
	has_metadata_location_ = true;
	return *this;
}

RegisterTableRequestBuilder &RegisterTableRequestBuilder::SetOverwrite(bool value) {
	result_.overwrite = std::move(value);
	return *this;
}

string RegisterTableRequestBuilder::TryBuild(RegisterTableRequest &result) {
	if (!has_name_) {
		return "RegisterTableRequest required property 'name' is missing";
	}
	if (!has_metadata_location_) {
		return "RegisterTableRequest required property 'metadata-location' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RegisterTableRequest RegisterTableRequestBuilder::Build() {
	RegisterTableRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RegisterTableRequest RegisterTableRequest::FromJSON(yyjson_val *obj) {
	RegisterTableRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RegisterTableRequest RegisterTableRequest::Copy() const {
	RegisterTableRequest res;
	res.name = name;
	res.metadata_location = metadata_location;
	if (overwrite.has_value()) {
		res.overwrite.emplace();
		(*res.overwrite) = (*overwrite);
	}
	return res;
}

string RegisterTableRequest::Validate() const {
	string error;
	return "";
}

string RegisterTableRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	auto name_val = yyjson_obj_get(obj, "name");
	if (!name_val) {
		return "RegisterTableRequest required property 'name' is missing";
	} else {
		if (yyjson_is_str(name_val)) {
			name = yyjson_get_str(name_val);
		} else {
			return StringUtil::Format(
			    "RegisterTableRequest property 'name' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(name_val));
		}
	}
	auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
	if (!metadata_location_val) {
		return "RegisterTableRequest required property 'metadata-location' is missing";
	} else {
		if (yyjson_is_str(metadata_location_val)) {
			metadata_location = yyjson_get_str(metadata_location_val);
		} else {
			return StringUtil::Format(
			    "RegisterTableRequest property 'metadata_location' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(metadata_location_val));
		}
	}
	auto overwrite_val = yyjson_obj_get(obj, "overwrite");
	if (overwrite_val) {
		bool overwrite_tmp;
		if (yyjson_is_bool(overwrite_val)) {
			overwrite_tmp = yyjson_get_bool(overwrite_val);
		} else {
			return StringUtil::Format(
			    "RegisterTableRequest property 'overwrite_tmp' is not of type 'boolean', found '%s' instead",
			    yyjson_get_type_desc(overwrite_val));
		}
		overwrite = std::move(overwrite_tmp);
	}
	return Validate();
}

void RegisterTableRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: metadata-location
	yyjson_mut_obj_add_strcpy(doc, obj, "metadata-location", metadata_location.c_str());

	// Serialize: overwrite
	if (overwrite.has_value()) {
		auto &overwrite_value = *overwrite;
		yyjson_mut_obj_add_bool(doc, obj, "overwrite", overwrite_value);
	}
}

yyjson_mut_val *RegisterTableRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
