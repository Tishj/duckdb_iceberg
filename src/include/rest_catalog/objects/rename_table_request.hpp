
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/table_identifier.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RenameTableRequest {
public:
	RenameTableRequest();
	RenameTableRequest(const RenameTableRequest &) = delete;
	RenameTableRequest &operator=(const RenameTableRequest &) = delete;
	RenameTableRequest(RenameTableRequest &&) = default;
	RenameTableRequest &operator=(RenameTableRequest &&) = default;

public:
	// Deserialization
	static RenameTableRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(RenameTableRequest &result);
	RenameTableRequest Build();

private:
	RenameTableRequest result_;
	bool has_source_ = false;
	bool has_destination_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
