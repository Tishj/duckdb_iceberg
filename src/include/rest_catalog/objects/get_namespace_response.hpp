
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

class GetNamespaceResponseBuilder;

class GetNamespaceResponse {
public:
	GetNamespaceResponse(const GetNamespaceResponse &) = delete;
	GetNamespaceResponse &operator=(const GetNamespaceResponse &) = delete;
	GetNamespaceResponse(GetNamespaceResponse &&) = default;
	GetNamespaceResponse &operator=(GetNamespaceResponse &&) = delete;

private:
	friend class GetNamespaceResponseBuilder;
	GetNamespaceResponse(Namespace _namespace_p, optional<case_insensitive_map_t<string>> properties_p);

public:
	// Deserialization
	static GetNamespaceResponse FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<GetNamespaceResponse> &result);
	string Validate() const;

	// Copy
	GetNamespaceResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	Namespace _namespace;
	optional<case_insensitive_map_t<string>> properties;
};

class GetNamespaceResponseBuilder {
public:
	GetNamespaceResponseBuilder();
	GetNamespaceResponseBuilder &SetNamespace(Namespace value);
	GetNamespaceResponseBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(optional<GetNamespaceResponse> &result);
	GetNamespaceResponse Build();

private:
	optional<Namespace> _namespace_;
	optional<case_insensitive_map_t<string>> properties_;
	bool has__namespace_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
