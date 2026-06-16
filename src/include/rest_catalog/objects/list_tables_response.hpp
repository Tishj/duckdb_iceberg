
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/page_token.hpp"
#include "rest_catalog/objects/table_identifier.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ListTablesResponseBuilder;

class ListTablesResponse {
public:
	ListTablesResponse(const ListTablesResponse &) = delete;
	ListTablesResponse &operator=(const ListTablesResponse &) = delete;
	ListTablesResponse(ListTablesResponse &&) = default;
	ListTablesResponse &operator=(ListTablesResponse &&) = delete;

private:
	friend class ListTablesResponseBuilder;
	ListTablesResponse(optional<PageToken> next_page_token_p, optional<vector<TableIdentifier>> identifiers_p);

public:
	// Deserialization
	static ListTablesResponse FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ListTablesResponseBuilder &builder);
	string Validate() const;

	// Copy
	ListTablesResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<PageToken> next_page_token;
	optional<vector<TableIdentifier>> identifiers;
};

class ListTablesResponseBuilder {
public:
	ListTablesResponseBuilder();
	ListTablesResponseBuilder &SetNextPageToken(PageToken value);
	ListTablesResponseBuilder &SetIdentifiers(vector<TableIdentifier> value);
	string TryBuild(optional<ListTablesResponse> &result);
	ListTablesResponse Build();

private:
	optional<PageToken> next_page_token_;
	optional<vector<TableIdentifier>> identifiers_;
};

} // namespace rest_api_objects
} // namespace duckdb
