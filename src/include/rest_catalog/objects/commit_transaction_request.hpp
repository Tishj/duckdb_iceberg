
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
	CommitTransactionRequest(const CommitTransactionRequest &);
	CommitTransactionRequest &operator=(const CommitTransactionRequest &) = delete;
	CommitTransactionRequest(CommitTransactionRequest &&);
	CommitTransactionRequest &operator=(CommitTransactionRequest &&) = delete;

private:
	friend class CommitTransactionRequestBuilder;
	CommitTransactionRequest(vector<CommitTableRequest> table_changes_p);

public:
	// Deserialization
	static CommitTransactionRequest FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CommitTransactionRequestBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CommitTransactionRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const vector<CommitTableRequest> table_changes;
};

class CommitTransactionRequestBuilder {
public:
	CommitTransactionRequestBuilder();
	CommitTransactionRequestBuilder &SetTableChanges(vector<CommitTableRequest> value);
	optional<string> TryBuild(optional<CommitTransactionRequest> &result);
	CommitTransactionRequest Build();

private:
	optional<vector<CommitTableRequest>> table_changes_;
	bool has_table_changes_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
