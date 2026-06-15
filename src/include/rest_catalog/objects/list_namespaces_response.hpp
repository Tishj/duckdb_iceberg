
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/namespace.hpp"
#include "rest_catalog/objects/page_token.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ListNamespacesResponseBuilder;

class ListNamespacesResponse {
public:
	ListNamespacesResponse(const ListNamespacesResponse &) = delete;
	ListNamespacesResponse &operator=(const ListNamespacesResponse &) = delete;
	ListNamespacesResponse(ListNamespacesResponse &&) = default;
	ListNamespacesResponse &operator=(ListNamespacesResponse &&) = default;

private:
	friend class ListNamespacesResponseBuilder;
	friend class GeneratedObjectAccess;
	ListNamespacesResponse();

public:
	// Deserialization
	static ListNamespacesResponse FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	ListNamespacesResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<PageToken> next_page_token;
	optional<vector<Namespace>> namespaces;
};

class ListNamespacesResponseBuilder {
public:
	ListNamespacesResponseBuilder();
	ListNamespacesResponseBuilder &SetNextPageToken(PageToken value);
	ListNamespacesResponseBuilder &SetNamespaces(vector<Namespace> value);
	string TryBuild(ListNamespacesResponse &result);
	ListNamespacesResponse Build();

private:
	ListNamespacesResponse result_;
};

} // namespace rest_api_objects
} // namespace duckdb
