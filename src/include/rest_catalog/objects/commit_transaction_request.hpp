
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/commit_table_request.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitTransactionRequestBuilder;

class CommitTransactionRequest {
public:
	CommitTransactionRequest(const CommitTransactionRequest &) = delete;
	CommitTransactionRequest &operator=(const CommitTransactionRequest &) = delete;
	CommitTransactionRequest(CommitTransactionRequest &&) = default;
	CommitTransactionRequest &operator=(CommitTransactionRequest &&) = default;

private:
	friend class CommitTransactionRequestBuilder;
	friend class GeneratedObjectAccess;
	CommitTransactionRequest();

public:
	// Deserialization
	static CommitTransactionRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CommitTransactionRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<CommitTableRequest> table_changes;
};

class CommitTransactionRequestBuilder {
public:
	CommitTransactionRequestBuilder();
	CommitTransactionRequestBuilder &SetTableChanges(vector<CommitTableRequest> value);
	string TryBuild(CommitTransactionRequest &result);
	CommitTransactionRequest Build();

private:
	CommitTransactionRequest result_;
	bool has_table_changes_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
