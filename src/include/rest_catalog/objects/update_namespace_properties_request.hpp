
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class UpdateNamespacePropertiesRequestBuilder;

class UpdateNamespacePropertiesRequest {
public:
	UpdateNamespacePropertiesRequest(const UpdateNamespacePropertiesRequest &) = delete;
	UpdateNamespacePropertiesRequest &operator=(const UpdateNamespacePropertiesRequest &) = delete;
	UpdateNamespacePropertiesRequest(UpdateNamespacePropertiesRequest &&) = default;
	UpdateNamespacePropertiesRequest &operator=(UpdateNamespacePropertiesRequest &&) = delete;

private:
	friend class UpdateNamespacePropertiesRequestBuilder;
	UpdateNamespacePropertiesRequest(optional<vector<string>> removals_p,
	                                 optional<case_insensitive_map_t<string>> updates_p);

public:
	// Deserialization
	static UpdateNamespacePropertiesRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<UpdateNamespacePropertiesRequest> &result);
	string Validate() const;

	// Copy
	UpdateNamespacePropertiesRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<vector<string>> removals;
	optional<case_insensitive_map_t<string>> updates;
};

class UpdateNamespacePropertiesRequestBuilder {
public:
	UpdateNamespacePropertiesRequestBuilder();
	UpdateNamespacePropertiesRequestBuilder &SetRemovals(vector<string> value);
	UpdateNamespacePropertiesRequestBuilder &SetUpdates(case_insensitive_map_t<string> value);
	string TryBuild(optional<UpdateNamespacePropertiesRequest> &result);
	UpdateNamespacePropertiesRequest Build();

private:
	optional<vector<string>> removals_;
	optional<case_insensitive_map_t<string>> updates_;
};

} // namespace rest_api_objects
} // namespace duckdb
