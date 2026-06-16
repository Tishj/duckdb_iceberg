
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/add_schema_update.hpp"
#include "rest_catalog/objects/add_view_version_update.hpp"
#include "rest_catalog/objects/assign_uuidupdate.hpp"
#include "rest_catalog/objects/remove_properties_update.hpp"
#include "rest_catalog/objects/set_current_view_version_update.hpp"
#include "rest_catalog/objects/set_location_update.hpp"
#include "rest_catalog/objects/set_properties_update.hpp"
#include "rest_catalog/objects/upgrade_format_version_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ViewUpdateBuilder;

class ViewUpdate {
public:
	ViewUpdate(const ViewUpdate &);
	ViewUpdate &operator=(const ViewUpdate &) = delete;
	ViewUpdate(ViewUpdate &&);
	ViewUpdate &operator=(ViewUpdate &&) = delete;

private:
	friend class ViewUpdateBuilder;
	ViewUpdate(optional<AssignUUIDUpdate> assign_uuidupdate_p,
	           optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_p,
	           optional<AddSchemaUpdate> add_schema_update_p, optional<SetLocationUpdate> set_location_update_p,
	           optional<SetPropertiesUpdate> set_properties_update_p,
	           optional<RemovePropertiesUpdate> remove_properties_update_p,
	           optional<AddViewVersionUpdate> add_view_version_update_p,
	           optional<SetCurrentViewVersionUpdate> set_current_view_version_update_p);

public:
	// Deserialization
	static ViewUpdate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, ViewUpdateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	ViewUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<AssignUUIDUpdate> assign_uuidupdate;
	const optional<UpgradeFormatVersionUpdate> upgrade_format_version_update;
	const optional<AddSchemaUpdate> add_schema_update;
	const optional<SetLocationUpdate> set_location_update;
	const optional<SetPropertiesUpdate> set_properties_update;
	const optional<RemovePropertiesUpdate> remove_properties_update;
	const optional<AddViewVersionUpdate> add_view_version_update;
	const optional<SetCurrentViewVersionUpdate> set_current_view_version_update;
};

class ViewUpdateBuilder {
public:
	ViewUpdateBuilder();
	ViewUpdateBuilder &SetAssignUuidupdate(AssignUUIDUpdate value);
	ViewUpdateBuilder &SetUpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate value);
	ViewUpdateBuilder &SetAddSchemaUpdate(AddSchemaUpdate value);
	ViewUpdateBuilder &SetSetLocationUpdate(SetLocationUpdate value);
	ViewUpdateBuilder &SetSetPropertiesUpdate(SetPropertiesUpdate value);
	ViewUpdateBuilder &SetRemovePropertiesUpdate(RemovePropertiesUpdate value);
	ViewUpdateBuilder &SetAddViewVersionUpdate(AddViewVersionUpdate value);
	ViewUpdateBuilder &SetSetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate value);
	optional<string> TryBuild(optional<ViewUpdate> &result);
	ViewUpdate Build();

private:
	optional<AssignUUIDUpdate> assign_uuidupdate_;
	optional<UpgradeFormatVersionUpdate> upgrade_format_version_update_;
	optional<AddSchemaUpdate> add_schema_update_;
	optional<SetLocationUpdate> set_location_update_;
	optional<SetPropertiesUpdate> set_properties_update_;
	optional<RemovePropertiesUpdate> remove_properties_update_;
	optional<AddViewVersionUpdate> add_view_version_update_;
	optional<SetCurrentViewVersionUpdate> set_current_view_version_update_;
};

} // namespace rest_api_objects
} // namespace duckdb
