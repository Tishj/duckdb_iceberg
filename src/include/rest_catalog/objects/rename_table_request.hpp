
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_identifier.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RenameTableRequestBuilder;

class RenameTableRequest {
public:
	RenameTableRequest(const RenameTableRequest &) = delete;
	RenameTableRequest &operator=(const RenameTableRequest &) = delete;
	RenameTableRequest(RenameTableRequest &&) = default;
	RenameTableRequest &operator=(RenameTableRequest &&) = delete;

private:
	friend class RenameTableRequestBuilder;
	RenameTableRequest(TableIdentifier source_p, TableIdentifier destination_p);

public:
	// Deserialization
	static RenameTableRequest FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<RenameTableRequest> &result);
	string Validate() const;

	// Copy
	RenameTableRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	TableIdentifier source;
	TableIdentifier destination;
};

class RenameTableRequestBuilder {
public:
	RenameTableRequestBuilder();
	RenameTableRequestBuilder &SetSource(TableIdentifier value);
	RenameTableRequestBuilder &SetDestination(TableIdentifier value);
	string TryBuild(optional<RenameTableRequest> &result);
	RenameTableRequest Build();

private:
	optional<TableIdentifier> source_;
	optional<TableIdentifier> destination_;
	bool has_source_ = false;
	bool has_destination_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
