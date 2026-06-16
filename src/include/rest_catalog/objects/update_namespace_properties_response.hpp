
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

class UpdateNamespacePropertiesResponseBuilder;

class UpdateNamespacePropertiesResponse {
public:
	UpdateNamespacePropertiesResponse(const UpdateNamespacePropertiesResponse &) = delete;
	UpdateNamespacePropertiesResponse &operator=(const UpdateNamespacePropertiesResponse &) = delete;
	UpdateNamespacePropertiesResponse(UpdateNamespacePropertiesResponse &&) = default;
	UpdateNamespacePropertiesResponse &operator=(UpdateNamespacePropertiesResponse &&) = delete;

private:
	friend class UpdateNamespacePropertiesResponseBuilder;
	UpdateNamespacePropertiesResponse(vector<string> updated_p, vector<string> removed_p,
	                                  optional<vector<string>> missing_p);

public:
	// Deserialization
	static UpdateNamespacePropertiesResponse FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, UpdateNamespacePropertiesResponseBuilder &builder);
	string Validate() const;

	// Copy
	UpdateNamespacePropertiesResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<string> updated;
	vector<string> removed;
	optional<vector<string>> missing;
};

class UpdateNamespacePropertiesResponseBuilder {
public:
	UpdateNamespacePropertiesResponseBuilder();
	UpdateNamespacePropertiesResponseBuilder &SetUpdated(vector<string> value);
	UpdateNamespacePropertiesResponseBuilder &SetRemoved(vector<string> value);
	UpdateNamespacePropertiesResponseBuilder &SetMissing(vector<string> value);
	string TryBuild(optional<UpdateNamespacePropertiesResponse> &result);
	UpdateNamespacePropertiesResponse Build();

private:
	optional<vector<string>> updated_;
	optional<vector<string>> removed_;
	optional<vector<string>> missing_;
	bool has_updated_ = false;
	bool has_removed_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
