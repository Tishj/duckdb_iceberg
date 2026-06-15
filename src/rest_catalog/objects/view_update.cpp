
#include "rest_catalog/objects/view_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ViewUpdate::ViewUpdate()
    : assign_uuidupdate(GeneratedObjectAccess::Create<optional<AssignUUIDUpdate>>()),
      upgrade_format_version_update(GeneratedObjectAccess::Create<optional<UpgradeFormatVersionUpdate>>()),
      add_schema_update(GeneratedObjectAccess::Create<optional<AddSchemaUpdate>>()),
      set_location_update(GeneratedObjectAccess::Create<optional<SetLocationUpdate>>()),
      set_properties_update(GeneratedObjectAccess::Create<optional<SetPropertiesUpdate>>()),
      remove_properties_update(GeneratedObjectAccess::Create<optional<RemovePropertiesUpdate>>()),
      add_view_version_update(GeneratedObjectAccess::Create<optional<AddViewVersionUpdate>>()),
      set_current_view_version_update(GeneratedObjectAccess::Create<optional<SetCurrentViewVersionUpdate>>()) {
}

ViewUpdateBuilder::ViewUpdateBuilder() {
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAssignUuidupdate(AssignUUIDUpdate value) {
	result_.assign_uuidupdate = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value) {
	result_.upgrade_format_version_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAddSchemaUpdate(AddSchemaUpdate value) {
	result_.add_schema_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetLocationUpdate(SetLocationUpdate value) {
	result_.set_location_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetPropertiesUpdate(SetPropertiesUpdate value) {
	result_.set_properties_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetRemovePropertiesUpdate(RemovePropertiesUpdate value) {
	result_.remove_properties_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAddViewVersionUpdate(AddViewVersionUpdate value) {
	result_.add_view_version_update = std::move(value);
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate value) {
	result_.set_current_view_version_update = std::move(value);
	return *this;
}

string ViewUpdateBuilder::TryBuild(ViewUpdate &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

ViewUpdate ViewUpdateBuilder::Build() {
	ViewUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

ViewUpdate ViewUpdate::FromJSON(yyjson_val *obj) {
	ViewUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

ViewUpdate ViewUpdate::Copy() const {
	ViewUpdate res;
	if (assign_uuidupdate.has_value()) {
		res.assign_uuidupdate = GeneratedObjectAccess::Create<AssignUUIDUpdate>();
		(*res.assign_uuidupdate) = (*assign_uuidupdate).Copy();
	}
	if (upgrade_format_version_update.has_value()) {
		res.upgrade_format_version_update = GeneratedObjectAccess::Create<UpgradeFormatVersionUpdate>();
		(*res.upgrade_format_version_update) = (*upgrade_format_version_update).Copy();
	}
	if (add_schema_update.has_value()) {
		res.add_schema_update = GeneratedObjectAccess::Create<AddSchemaUpdate>();
		(*res.add_schema_update) = (*add_schema_update).Copy();
	}
	if (set_location_update.has_value()) {
		res.set_location_update = GeneratedObjectAccess::Create<SetLocationUpdate>();
		(*res.set_location_update) = (*set_location_update).Copy();
	}
	if (set_properties_update.has_value()) {
		res.set_properties_update = GeneratedObjectAccess::Create<SetPropertiesUpdate>();
		(*res.set_properties_update) = (*set_properties_update).Copy();
	}
	if (remove_properties_update.has_value()) {
		res.remove_properties_update = GeneratedObjectAccess::Create<RemovePropertiesUpdate>();
		(*res.remove_properties_update) = (*remove_properties_update).Copy();
	}
	if (add_view_version_update.has_value()) {
		res.add_view_version_update = GeneratedObjectAccess::Create<AddViewVersionUpdate>();
		(*res.add_view_version_update) = (*add_view_version_update).Copy();
	}
	if (set_current_view_version_update.has_value()) {
		res.set_current_view_version_update = GeneratedObjectAccess::Create<SetCurrentViewVersionUpdate>();
		(*res.set_current_view_version_update) = (*set_current_view_version_update).Copy();
	}
	return res;
}

string ViewUpdate::Validate() const {
	string error;
	int matched_any_of_variants = 0;
	if (assign_uuidupdate.has_value()) {
		matched_any_of_variants++;
		error = assign_uuidupdate->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (upgrade_format_version_update.has_value()) {
		matched_any_of_variants++;
		error = upgrade_format_version_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_schema_update.has_value()) {
		matched_any_of_variants++;
		error = add_schema_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_location_update.has_value()) {
		matched_any_of_variants++;
		error = set_location_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_properties_update.has_value()) {
		matched_any_of_variants++;
		error = set_properties_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (remove_properties_update.has_value()) {
		matched_any_of_variants++;
		error = remove_properties_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (add_view_version_update.has_value()) {
		matched_any_of_variants++;
		error = add_view_version_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_current_view_version_update.has_value()) {
		matched_any_of_variants++;
		error = set_current_view_version_update->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "ViewUpdate must have at least one anyOf variant set";
	}
	return "";
}

string ViewUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	assign_uuidupdate = GeneratedObjectAccess::Create<AssignUUIDUpdate>();
	error = assign_uuidupdate->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		assign_uuidupdate = nullopt;
	}
	upgrade_format_version_update = GeneratedObjectAccess::Create<UpgradeFormatVersionUpdate>();
	error = upgrade_format_version_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		upgrade_format_version_update = nullopt;
	}
	add_schema_update = GeneratedObjectAccess::Create<AddSchemaUpdate>();
	error = add_schema_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_schema_update = nullopt;
	}
	set_location_update = GeneratedObjectAccess::Create<SetLocationUpdate>();
	error = set_location_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_location_update = nullopt;
	}
	set_properties_update = GeneratedObjectAccess::Create<SetPropertiesUpdate>();
	error = set_properties_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_properties_update = nullopt;
	}
	remove_properties_update = GeneratedObjectAccess::Create<RemovePropertiesUpdate>();
	error = remove_properties_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		remove_properties_update = nullopt;
	}
	add_view_version_update = GeneratedObjectAccess::Create<AddViewVersionUpdate>();
	error = add_view_version_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		add_view_version_update = nullopt;
	}
	set_current_view_version_update = GeneratedObjectAccess::Create<SetCurrentViewVersionUpdate>();
	error = set_current_view_version_update->TryFromJSON(obj);
	if (error.empty()) {
	} else {
		set_current_view_version_update = nullopt;
	}
	if (!(add_schema_update.has_value()) && !(add_view_version_update.has_value()) &&
	    !(assign_uuidupdate.has_value()) && !(remove_properties_update.has_value()) &&
	    !(set_current_view_version_update.has_value()) && !(set_location_update.has_value()) &&
	    !(set_properties_update.has_value()) && !(upgrade_format_version_update.has_value())) {
		return "ViewUpdate failed to parse, none of the anyOf candidates matched";
	}
	return Validate();
}

void ViewUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (assign_uuidupdate.has_value()) {
		assign_uuidupdate->PopulateJSON(doc, obj);
	} else if (upgrade_format_version_update.has_value()) {
		upgrade_format_version_update->PopulateJSON(doc, obj);
	} else if (add_schema_update.has_value()) {
		add_schema_update->PopulateJSON(doc, obj);
	} else if (set_location_update.has_value()) {
		set_location_update->PopulateJSON(doc, obj);
	} else if (set_properties_update.has_value()) {
		set_properties_update->PopulateJSON(doc, obj);
	} else if (remove_properties_update.has_value()) {
		remove_properties_update->PopulateJSON(doc, obj);
	} else if (add_view_version_update.has_value()) {
		add_view_version_update->PopulateJSON(doc, obj);
	} else if (set_current_view_version_update.has_value()) {
		set_current_view_version_update->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *ViewUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
