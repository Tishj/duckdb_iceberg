
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
ViewUpdate::ViewUpdate(const ViewUpdate &other)
    : assign_uuidupdate((other.assign_uuidupdate.has_value()
                             ? optional<AssignUUIDUpdate>((*other.assign_uuidupdate).Copy())
                             : optional<AssignUUIDUpdate>())),
      upgrade_format_version_update(
          (other.upgrade_format_version_update.has_value()
               ? optional<UpgradeFormatVersionUpdate>((*other.upgrade_format_version_update).Copy())
               : optional<UpgradeFormatVersionUpdate>())),
      add_schema_update((other.add_schema_update.has_value()
                             ? optional<AddSchemaUpdate>((*other.add_schema_update).Copy())
                             : optional<AddSchemaUpdate>())),
      set_location_update((other.set_location_update.has_value()
                               ? optional<SetLocationUpdate>((*other.set_location_update).Copy())
                               : optional<SetLocationUpdate>())),
      set_properties_update((other.set_properties_update.has_value()
                                 ? optional<SetPropertiesUpdate>((*other.set_properties_update).Copy())
                                 : optional<SetPropertiesUpdate>())),
      remove_properties_update((other.remove_properties_update.has_value()
                                    ? optional<RemovePropertiesUpdate>((*other.remove_properties_update).Copy())
                                    : optional<RemovePropertiesUpdate>())),
      add_view_version_update((other.add_view_version_update.has_value()
                                   ? optional<AddViewVersionUpdate>((*other.add_view_version_update).Copy())
                                   : optional<AddViewVersionUpdate>())),
      set_current_view_version_update(
          (other.set_current_view_version_update.has_value()
               ? optional<SetCurrentViewVersionUpdate>((*other.set_current_view_version_update).Copy())
               : optional<SetCurrentViewVersionUpdate>())) {
}
ViewUpdate::ViewUpdate(ViewUpdate &&other) : ViewUpdate(static_cast<const ViewUpdate &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> ViewUpdateBuilder::TryBuild(optional<ViewUpdate> &result) {
	auto built = ViewUpdate(std::move(assign_uuidupdate_), std::move(upgrade_format_version_update_),
	                        std::move(add_schema_update_), std::move(set_location_update_),
	                        std::move(set_properties_update_), std::move(remove_properties_update_),
	                        std::move(add_view_version_update_), std::move(set_current_view_version_update_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> ViewUpdate::TryFromJSON(yyjson_val *obj, ViewUpdateBuilder &builder) {
	try {
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewUpdate ViewUpdate::FromJSON(yyjson_val *obj) {
	ViewUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

ViewUpdate ViewUpdate::Copy() const {
	return ViewUpdate(*this);
}

optional<string> ViewUpdate::Validate() const {
	optional<string> error;
	int matched_any_of_variants = 0;
	if (assign_uuidupdate.has_value()) {
		matched_any_of_variants++;
		error = assign_uuidupdate->Validate();
		if (error) {
			return error;
		}
	}
	if (upgrade_format_version_update.has_value()) {
		matched_any_of_variants++;
		error = upgrade_format_version_update->Validate();
		if (error) {
			return error;
		}
	}
	if (add_schema_update.has_value()) {
		matched_any_of_variants++;
		error = add_schema_update->Validate();
		if (error) {
			return error;
		}
	}
	if (set_location_update.has_value()) {
		matched_any_of_variants++;
		error = set_location_update->Validate();
		if (error) {
			return error;
		}
	}
	if (set_properties_update.has_value()) {
		matched_any_of_variants++;
		error = set_properties_update->Validate();
		if (error) {
			return error;
		}
	}
	if (remove_properties_update.has_value()) {
		matched_any_of_variants++;
		error = remove_properties_update->Validate();
		if (error) {
			return error;
		}
	}
	if (add_view_version_update.has_value()) {
		matched_any_of_variants++;
		error = add_view_version_update->Validate();
		if (error) {
			return error;
		}
	}
	if (set_current_view_version_update.has_value()) {
		matched_any_of_variants++;
		error = set_current_view_version_update->Validate();
		if (error) {
			return error;
		}
	}
	if (matched_any_of_variants == 0) {
		return "ViewUpdate must have at least one anyOf variant set";
	}
	return nullopt;
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
