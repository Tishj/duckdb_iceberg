
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/schema.hpp"
#include "rest_catalog/objects/view_history_entry.hpp"
#include "rest_catalog/objects/view_version.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ViewMetadataBuilder;

class ViewMetadata {
public:
	ViewMetadata(const ViewMetadata &) = delete;
	ViewMetadata &operator=(const ViewMetadata &) = delete;
	ViewMetadata(ViewMetadata &&) = default;
	ViewMetadata &operator=(ViewMetadata &&) = delete;

private:
	friend class ViewMetadataBuilder;
	ViewMetadata(string view_uuid_p, int32_t format_version_p, string location_p, int32_t current_version_id_p,
	             vector<ViewVersion> versions_p, vector<ViewHistoryEntry> version_log_p, vector<Schema> schemas_p,
	             optional<case_insensitive_map_t<string>> properties_p);

public:
	// Deserialization
	static ViewMetadata FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ViewMetadataBuilder &builder);
	string Validate() const;

	// Copy
	ViewMetadata Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string view_uuid;
	int32_t format_version;
	string location;
	int32_t current_version_id;
	vector<ViewVersion> versions;
	vector<ViewHistoryEntry> version_log;
	vector<Schema> schemas;
	optional<case_insensitive_map_t<string>> properties;
};

class ViewMetadataBuilder {
public:
	ViewMetadataBuilder();
	ViewMetadataBuilder &SetViewUuid(string value);
	ViewMetadataBuilder &SetFormatVersion(int32_t value);
	ViewMetadataBuilder &SetLocation(string value);
	ViewMetadataBuilder &SetCurrentVersionId(int32_t value);
	ViewMetadataBuilder &SetVersions(vector<ViewVersion> value);
	ViewMetadataBuilder &SetVersionLog(vector<ViewHistoryEntry> value);
	ViewMetadataBuilder &SetSchemas(vector<Schema> value);
	ViewMetadataBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(optional<ViewMetadata> &result);
	ViewMetadata Build();

private:
	optional<string> view_uuid_;
	optional<int32_t> format_version_;
	optional<string> location_;
	optional<int32_t> current_version_id_;
	optional<vector<ViewVersion>> versions_;
	optional<vector<ViewHistoryEntry>> version_log_;
	optional<vector<Schema>> schemas_;
	optional<case_insensitive_map_t<string>> properties_;
	bool has_view_uuid_ = false;
	bool has_format_version_ = false;
	bool has_location_ = false;
	bool has_current_version_id_ = false;
	bool has_versions_ = false;
	bool has_version_log_ = false;
	bool has_schemas_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
