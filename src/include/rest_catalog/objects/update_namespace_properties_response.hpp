
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class UpdateNamespacePropertiesResponse {
public:
	UpdateNamespacePropertiesResponse();
	UpdateNamespacePropertiesResponse(const UpdateNamespacePropertiesResponse &) = delete;
	UpdateNamespacePropertiesResponse &operator=(const UpdateNamespacePropertiesResponse &) = delete;
	UpdateNamespacePropertiesResponse(UpdateNamespacePropertiesResponse &&) = default;
	UpdateNamespacePropertiesResponse &operator=(UpdateNamespacePropertiesResponse &&) = default;

public:
	// Deserialization
	static UpdateNamespacePropertiesResponse FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	UpdateNamespacePropertiesResponse Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<string> updated;
	vector<string> removed;
	optional<vector<string>> missing;
};

class UpdateNamespacePropertiesResponseBuilder {
public:
	UpdateNamespacePropertiesResponseBuilder();
	UpdateNamespacePropertiesResponseBuilder &SetUpdated(vector<string> value);
	UpdateNamespacePropertiesResponseBuilder &SetRemoved(vector<string> value);
	UpdateNamespacePropertiesResponseBuilder &SetMissing(vector<string> value);
	string TryBuild(UpdateNamespacePropertiesResponse &result);
	UpdateNamespacePropertiesResponse Build();

private:
	UpdateNamespacePropertiesResponse result_;
	bool has_updated_ = false;
	bool has_removed_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
