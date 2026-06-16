
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
	BaseUpdate(const BaseUpdate &);
	BaseUpdate &operator=(const BaseUpdate &) = delete;
	BaseUpdate(BaseUpdate &&);
	BaseUpdate &operator=(BaseUpdate &&) = delete;

private:
	friend class BaseUpdateBuilder;
	BaseUpdate(string action_p);

public:
	// Deserialization
	static BaseUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, BaseUpdateBuilder &builder);
	string Validate() const;

	// Copy
	BaseUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string action;
};

class BaseUpdateBuilder {
public:
	BaseUpdateBuilder();
	BaseUpdateBuilder &SetAction(string value);
	string TryBuild(optional<BaseUpdate> &result);
	BaseUpdate Build();

private:
	optional<string> action_;
	bool has_action_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
