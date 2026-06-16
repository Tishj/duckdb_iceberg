
#include "rest_catalog/objects/set_location_update.hpp"

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

SetLocationUpdate::SetLocationUpdate(BaseUpdate base_update_p, string location_p)
    : base_update(std::move(base_update_p)), location(std::move(location_p)) {
}
SetLocationUpdate::SetLocationUpdate(const SetLocationUpdate &other)
    : base_update(other.base_update.Copy()), location(other.location) {
}
SetLocationUpdate::SetLocationUpdate(SetLocationUpdate &&other)
    : SetLocationUpdate(static_cast<const SetLocationUpdate &>(other)) {
}

SetLocationUpdateBuilder::SetLocationUpdateBuilder() {
}

SetLocationUpdateBuilder &SetLocationUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetLocationUpdateBuilder &SetLocationUpdateBuilder::SetLocation(string value) {
	location_.emplace(std::move(value));
	has_location_ = true;
	return *this;
}

SetLocationUpdate SetLocationUpdateBuilder::Build() {
	if (!has_location_) {
		throw InvalidInputException("SetLocationUpdate required property 'location' is missing");
	}
	auto result = SetLocationUpdate(std::move(*base_update_), std::move(*location_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> SetLocationUpdateBuilder::TryBuild(optional<SetLocationUpdate> &result) {
	if (!has_location_) {
		return "SetLocationUpdate required property 'location' is missing";
	}
	auto built = SetLocationUpdate(std::move(*base_update_), std::move(*location_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> SetLocationUpdate::TryFromJSON(yyjson_val *obj, SetLocationUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto location_val = yyjson_obj_get(obj, "location");
		if (!location_val) {
			throw InvalidInputException("SetLocationUpdate required property 'location' is missing");
		} else {
			string location;
			if (yyjson_is_str(location_val)) {
				location = yyjson_get_str(location_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "SetLocationUpdate property 'location' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(location_val)));
			}
			builder.SetLocation(std::move(location));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetLocationUpdate SetLocationUpdate::FromJSON(yyjson_val *obj) {
	SetLocationUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

SetLocationUpdate SetLocationUpdate::Copy() const {
	return SetLocationUpdate(*this);
}

optional<string> SetLocationUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
