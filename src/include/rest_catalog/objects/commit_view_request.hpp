
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_identifier.hpp"
#include "rest_catalog/objects/view_requirement.hpp"
#include "rest_catalog/objects/view_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitViewRequestBuilder;

class CommitViewRequest {
public:
	CommitViewRequest(const CommitViewRequest &) = delete;
	CommitViewRequest &operator=(const CommitViewRequest &) = delete;
	CommitViewRequest(CommitViewRequest &&) = default;
	CommitViewRequest &operator=(CommitViewRequest &&) = default;

private:
	friend class CommitViewRequestBuilder;
	friend class GeneratedObjectAccess;
	CommitViewRequest();

public:
	// Deserialization
	static CommitViewRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CommitViewRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<ViewUpdate> updates;
	optional<TableIdentifier> identifier;
	optional<vector<ViewRequirement>> requirements;
};

class CommitViewRequestBuilder {
public:
	CommitViewRequestBuilder();
	CommitViewRequestBuilder &SetUpdates(vector<ViewUpdate> value);
	CommitViewRequestBuilder &SetIdentifier(TableIdentifier value);
	CommitViewRequestBuilder &SetRequirements(vector<ViewRequirement> value);
	string TryBuild(CommitViewRequest &result);
	CommitViewRequest Build();

private:
	CommitViewRequest result_;
	bool has_updates_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
