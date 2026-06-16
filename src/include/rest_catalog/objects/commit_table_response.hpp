
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_metadata.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitTableResponseBuilder;

class CommitTableResponse {
public:
	CommitTableResponse(const CommitTableResponse &);
	CommitTableResponse &operator=(const CommitTableResponse &) = delete;
	CommitTableResponse(CommitTableResponse &&);
	CommitTableResponse &operator=(CommitTableResponse &&) = delete;

private:
	friend class CommitTableResponseBuilder;
	CommitTableResponse(string metadata_location_p, TableMetadata metadata_p);

public:
	// Deserialization
	static CommitTableResponse FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CommitTableResponseBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CommitTableResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string metadata_location;
	const TableMetadata metadata;
};

class CommitTableResponseBuilder {
public:
	CommitTableResponseBuilder();
	CommitTableResponseBuilder &SetMetadataLocation(string value);
	CommitTableResponseBuilder &SetMetadata(TableMetadata value);
	optional<string> TryBuild(optional<CommitTableResponse> &result);
	CommitTableResponse Build();

private:
	optional<string> metadata_location_;
	optional<TableMetadata> metadata_;
	bool has_metadata_location_ = false;
	bool has_metadata_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
