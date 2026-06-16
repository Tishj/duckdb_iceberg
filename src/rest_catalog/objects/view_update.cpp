
#include "rest_catalog/objects/view_update.hpp"

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

ViewUpdate::ViewUpdate(optional<AssignUUIDUpdate> assign_uuidupdate_p,
                       optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_p,
                       optional<AddSchemaUpdate> add_schema_update_p, optional<SetLocationUpdate> set_location_update_p,
                       optional<SetPropertiesUpdate> set_properties_update_p,
                       optional<RemovePropertiesUpdate> remove_properties_update_p,
                       optional<AddViewVersionUpdate> add_view_version_update_p,
                       optional<SetCurrentViewVersionUpdate> set_current_view_version_update_p)
    : assign_uuidupdate(std::move(assign_uuidupdate_p)),
      upgrade_format_version_update(std::move(upgrade_format_version_update_p)),
      add_schema_update(std::move(add_schema_update_p)), set_location_update(std::move(set_location_update_p)),
      set_properties_update(std::move(set_properties_update_p)),
      remove_properties_update(std::move(remove_properties_update_p)),
      add_view_version_update(std::move(add_view_version_update_p)),
      set_current_view_version_update(std::move(set_current_view_version_update_p)) {
}

ViewUpdateBuilder::ViewUpdateBuilder() {
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAssignUuidupdate(AssignUUIDUpdate value) {
	assign_uuidupdate_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value) {
	upgrade_format_version_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAddSchemaUpdate(AddSchemaUpdate value) {
	add_schema_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetLocationUpdate(SetLocationUpdate value) {
	set_location_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetPropertiesUpdate(SetPropertiesUpdate value) {
	set_properties_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetRemovePropertiesUpdate(RemovePropertiesUpdate value) {
	remove_properties_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetAddViewVersionUpdate(AddViewVersionUpdate value) {
	add_view_version_update_.emplace(std::move(value));
	return *this;
}

ViewUpdateBuilder &ViewUpdateBuilder::SetSetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate value) {
	set_current_view_version_update_.emplace(std::move(value));
	return *this;
}

ViewUpdate ViewUpdateBuilder::Build() {
	auto result = ViewUpdate(std::move(assign_uuidupdate_), std::move(upgrade_format_version_update_),
	                         std::move(add_schema_update_), std::move(set_location_update_),
	                         std::move(set_properties_update_), std::move(remove_properties_update_),
	                         std::move(add_view_version_update_), std::move(set_current_view_version_update_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewUpdateBuilder::TryBuild(optional<ViewUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewUpdate ViewUpdate::FromJSON(yyjson_val *obj) {
	ViewUpdateBuilder builder;
	int matched_any_of_variants = 0;
	try {
		builder.SetAssignUuidupdate(AssignUUIDUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetAddSchemaUpdate(AddSchemaUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetSetLocationUpdate(SetLocationUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetSetPropertiesUpdate(SetPropertiesUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetRemovePropertiesUpdate(RemovePropertiesUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetAddViewVersionUpdate(AddViewVersionUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	try {
		builder.SetSetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate::FromJSON(obj));
		matched_any_of_variants++;
	} catch (const Exception &) {
	}
	if (matched_any_of_variants == 0) {
		throw InvalidInputException("ViewUpdate failed to parse, none of the anyOf candidates matched");
	}
	return builder.Build();
}

string ViewUpdate::TryFromJSON(yyjson_val *obj, optional<ViewUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewUpdate ViewUpdate::Copy() const {
	ViewUpdateBuilder builder;
	optional<AssignUUIDUpdate> assign_uuidupdate_tmp;
	if (assign_uuidupdate.has_value()) {
		assign_uuidupdate_tmp.emplace((*assign_uuidupdate).Copy());
	}
	if (assign_uuidupdate_tmp.has_value()) {
		builder.SetAssignUuidupdate(std::move(*assign_uuidupdate_tmp));
	}
	optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_tmp;
	if (upgrade_format_version_update.has_value()) {
		upgrade_format_version_update_tmp.emplace((*upgrade_format_version_update).Copy());
	}
	if (upgrade_format_version_update_tmp.has_value()) {
		builder.SetUpgradeFormatVersionUpdate(std::move(*upgrade_format_version_update_tmp));
	}
	optional<AddSchemaUpdate> add_schema_update_tmp;
	if (add_schema_update.has_value()) {
		add_schema_update_tmp.emplace((*add_schema_update).Copy());
	}
	if (add_schema_update_tmp.has_value()) {
		builder.SetAddSchemaUpdate(std::move(*add_schema_update_tmp));
	}
	optional<SetLocationUpdate> set_location_update_tmp;
	if (set_location_update.has_value()) {
		set_location_update_tmp.emplace((*set_location_update).Copy());
	}
	if (set_location_update_tmp.has_value()) {
		builder.SetSetLocationUpdate(std::move(*set_location_update_tmp));
	}
	optional<SetPropertiesUpdate> set_properties_update_tmp;
	if (set_properties_update.has_value()) {
		set_properties_update_tmp.emplace((*set_properties_update).Copy());
	}
	if (set_properties_update_tmp.has_value()) {
		builder.SetSetPropertiesUpdate(std::move(*set_properties_update_tmp));
	}
	optional<RemovePropertiesUpdate> remove_properties_update_tmp;
	if (remove_properties_update.has_value()) {
		remove_properties_update_tmp.emplace((*remove_properties_update).Copy());
	}
	if (remove_properties_update_tmp.has_value()) {
		builder.SetRemovePropertiesUpdate(std::move(*remove_properties_update_tmp));
	}
	optional<AddViewVersionUpdate> add_view_version_update_tmp;
	if (add_view_version_update.has_value()) {
		add_view_version_update_tmp.emplace((*add_view_version_update).Copy());
	}
	if (add_view_version_update_tmp.has_value()) {
		builder.SetAddViewVersionUpdate(std::move(*add_view_version_update_tmp));
	}
	optional<SetCurrentViewVersionUpdate> set_current_view_version_update_tmp;
	if (set_current_view_version_update.has_value()) {
		set_current_view_version_update_tmp.emplace((*set_current_view_version_update).Copy());
	}
	if (set_current_view_version_update_tmp.has_value()) {
		builder.SetSetCurrentViewVersionUpdate(std::move(*set_current_view_version_update_tmp));
	}
	return builder.Build();
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
