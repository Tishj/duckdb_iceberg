
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"
#include "rest_catalog/objects/view_version.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AddViewVersionUpdateBuilder;

class AddViewVersionUpdate {
public:
	AddViewVersionUpdate(const AddViewVersionUpdate &) = delete;
	AddViewVersionUpdate &operator=(const AddViewVersionUpdate &) = delete;
	AddViewVersionUpdate(AddViewVersionUpdate &&) = default;
	AddViewVersionUpdate &operator=(AddViewVersionUpdate &&) = default;

private:
	friend class AddViewVersionUpdateBuilder;
	friend class GeneratedObjectAccess;
	AddViewVersionUpdate();

public:
	// Deserialization
	static AddViewVersionUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	AddViewVersionUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	ViewVersion view_version;
};

class AddViewVersionUpdateBuilder {
public:
	AddViewVersionUpdateBuilder();
	AddViewVersionUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AddViewVersionUpdateBuilder &SetViewVersion(ViewVersion value);
	string TryBuild(AddViewVersionUpdate &result);
	AddViewVersionUpdate Build();

private:
	AddViewVersionUpdate result_;
	bool has_view_version_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
