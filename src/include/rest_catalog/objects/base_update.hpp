
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class BaseUpdateBuilder;

class BaseUpdate {
public:
	BaseUpdate(const BaseUpdate &) = delete;
	BaseUpdate &operator=(const BaseUpdate &) = delete;
	BaseUpdate(BaseUpdate &&) = default;
	BaseUpdate &operator=(BaseUpdate &&) = default;

private:
	friend class BaseUpdateBuilder;
	friend class GeneratedObjectAccess;
	BaseUpdate();

public:
	// Deserialization
	static BaseUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	BaseUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string action;
};

class BaseUpdateBuilder {
public:
	BaseUpdateBuilder();
	BaseUpdateBuilder &SetAction(string value);
	string TryBuild(BaseUpdate &result);
	BaseUpdate Build();

private:
	BaseUpdate result_;
	bool has_action_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
