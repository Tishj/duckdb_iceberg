
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class RemovePropertiesUpdateBuilder;

class RemovePropertiesUpdate {
public:
	RemovePropertiesUpdate(const RemovePropertiesUpdate &) = delete;
	RemovePropertiesUpdate &operator=(const RemovePropertiesUpdate &) = delete;
	RemovePropertiesUpdate(RemovePropertiesUpdate &&) = default;
	RemovePropertiesUpdate &operator=(RemovePropertiesUpdate &&) = delete;

private:
	friend class RemovePropertiesUpdateBuilder;
	RemovePropertiesUpdate(BaseUpdate base_update_p, vector<string> removals_p);

public:
	// Deserialization
	static RemovePropertiesUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<RemovePropertiesUpdate> &result);
	string Validate() const;

	// Copy
	RemovePropertiesUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	vector<string> removals;
};

class RemovePropertiesUpdateBuilder {
public:
	RemovePropertiesUpdateBuilder();
	RemovePropertiesUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	RemovePropertiesUpdateBuilder &SetRemovals(vector<string> value);
	string TryBuild(optional<RemovePropertiesUpdate> &result);
	RemovePropertiesUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<vector<string>> removals_;
	bool has_removals_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
