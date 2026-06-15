
#include "rest_catalog/objects/set_location_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SetLocationUpdate::SetLocationUpdate(BaseUpdate base_update_p, string location_p)
    : base_update(std::move(base_update_p)), location(std::move(location_p)) {
}

SetLocationUpdateBuilder::SetLocationUpdateBuilder() {
}

SetLocationUpdateBuilder &SetLocationUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

SetLocationUpdateBuilder &SetLocationUpdateBuilder::SetLocation(string value) {
	location_ = std::move(value);
	has_location_ = true;
	return *this;
}

SetLocationUpdate SetLocationUpdateBuilder::Build() {
	if (!has_location_) {
		throw InvalidInputException("SetLocationUpdate required property 'location' is missing");
	}
	auto result = SetLocationUpdate(std::move(*base_update_), std::move(*location_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetLocationUpdateBuilder::TryBuild(optional<SetLocationUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetLocationUpdate SetLocationUpdate::FromJSON(yyjson_val *obj) {
	SetLocationUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto location_val = yyjson_obj_get(obj, "location");
	if (!location_val) {
		throw InvalidInputException("SetLocationUpdate required property 'location' is missing");
	} else {
		string location;
		if (yyjson_is_str(location_val)) {
			location = yyjson_get_str(location_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SetLocationUpdate property 'location' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(location_val)));
		}
		builder.SetLocation(std::move(location));
	}
	return builder.Build();
}

string SetLocationUpdate::TryFromJSON(yyjson_val *obj, optional<SetLocationUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetLocationUpdate SetLocationUpdate::Copy() const {
	SetLocationUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	string location_tmp;
	location_tmp = location;
	builder.SetLocation(std::move(location_tmp));
	return builder.Build();
}

string SetLocationUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SetLocationUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: location
	yyjson_mut_obj_add_strcpy(doc, obj, "location", location.c_str());
}

yyjson_mut_val *SetLocationUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
