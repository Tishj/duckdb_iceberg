
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_identifier.hpp"
#include "rest_catalog/objects/table_requirement.hpp"
#include "rest_catalog/objects/table_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitTableRequestBuilder;

class CommitTableRequest {
public:
	CommitTableRequest(const CommitTableRequest &) = delete;
	CommitTableRequest &operator=(const CommitTableRequest &) = delete;
	CommitTableRequest(CommitTableRequest &&) = default;
	CommitTableRequest &operator=(CommitTableRequest &&) = delete;

private:
	friend class CommitTableRequestBuilder;
	CommitTableRequest(vector<TableRequirement> requirements_p, vector<TableUpdate> updates_p,
	                   optional<TableIdentifier> identifier_p);

public:
	// Deserialization
	static CommitTableRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<CommitTableRequest> &result);
	string Validate() const;

	// Copy
	CommitTableRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<TableRequirement> requirements;
	vector<TableUpdate> updates;
	optional<TableIdentifier> identifier;
};

class CommitTableRequestBuilder {
public:
	CommitTableRequestBuilder();
	CommitTableRequestBuilder &SetRequirements(vector<TableRequirement> value);
	CommitTableRequestBuilder &SetUpdates(vector<TableUpdate> value);
	CommitTableRequestBuilder &SetIdentifier(TableIdentifier value);
	string TryBuild(optional<CommitTableRequest> &result);
	CommitTableRequest Build();

private:
	optional<vector<TableRequirement>> requirements_;
	optional<vector<TableUpdate>> updates_;
	optional<TableIdentifier> identifier_;
	bool has_requirements_ = false;
	bool has_updates_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
