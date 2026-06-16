
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/namespace.hpp"
#include "rest_catalog/objects/view_representation.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ViewVersionBuilder;

class ViewVersion {
public:
	ViewVersion(const ViewVersion &);
	ViewVersion &operator=(const ViewVersion &) = delete;
	ViewVersion(ViewVersion &&);
	ViewVersion &operator=(ViewVersion &&) = delete;

private:
	friend class ViewVersionBuilder;
	ViewVersion(int32_t version_id_p, int64_t timestamp_ms_p, int32_t schema_id_p,
	            case_insensitive_map_t<string> summary_p, vector<ViewRepresentation> representations_p,
	            Namespace default_namespace_p, optional<string> default_catalog_p);

public:
	// Deserialization
	static ViewVersion FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ViewVersionBuilder &builder);
	string Validate() const;

	// Copy
	ViewVersion Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const int32_t version_id;
	const int64_t timestamp_ms;
	const int32_t schema_id;
	const case_insensitive_map_t<string> summary;
	const vector<ViewRepresentation> representations;
	const Namespace default_namespace;
	const optional<string> default_catalog;
};

class ViewVersionBuilder {
public:
	ViewVersionBuilder();
	ViewVersionBuilder &SetVersionId(int32_t value);
	ViewVersionBuilder &SetTimestampMs(int64_t value);
	ViewVersionBuilder &SetSchemaId(int32_t value);
	ViewVersionBuilder &SetSummary(case_insensitive_map_t<string> value);
	ViewVersionBuilder &SetRepresentations(vector<ViewRepresentation> value);
	ViewVersionBuilder &SetDefaultNamespace(Namespace value);
	ViewVersionBuilder &SetDefaultCatalog(string value);
	string TryBuild(optional<ViewVersion> &result);
	ViewVersion Build();

private:
	optional<int32_t> version_id_;
	optional<int64_t> timestamp_ms_;
	optional<int32_t> schema_id_;
	optional<case_insensitive_map_t<string>> summary_;
	optional<vector<ViewRepresentation>> representations_;
	optional<Namespace> default_namespace_;
	optional<string> default_catalog_;
	bool has_version_id_ = false;
	bool has_timestamp_ms_ = false;
	bool has_schema_id_ = false;
	bool has_summary_ = false;
	bool has_representations_ = false;
	bool has_default_namespace_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
