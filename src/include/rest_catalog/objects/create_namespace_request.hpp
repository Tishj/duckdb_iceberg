
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/namespace.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CreateNamespaceRequestBuilder;

class CreateNamespaceRequest {
public:
	CreateNamespaceRequest(const CreateNamespaceRequest &) = delete;
	CreateNamespaceRequest &operator=(const CreateNamespaceRequest &) = delete;
	CreateNamespaceRequest(CreateNamespaceRequest &&) = default;
	CreateNamespaceRequest &operator=(CreateNamespaceRequest &&) = default;

private:
	friend class CreateNamespaceRequestBuilder;
	friend class GeneratedObjectAccess;
	CreateNamespaceRequest();

public:
	// Deserialization
	static CreateNamespaceRequest FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CreateNamespaceRequest Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	Namespace _namespace;
	optional<case_insensitive_map_t<string>> properties;
};

class CreateNamespaceRequestBuilder {
public:
	CreateNamespaceRequestBuilder();
	CreateNamespaceRequestBuilder &SetNamespace(Namespace value);
	CreateNamespaceRequestBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(CreateNamespaceRequest &result);
	CreateNamespaceRequest Build();

private:
	CreateNamespaceRequest result_;
	bool has__namespace_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
