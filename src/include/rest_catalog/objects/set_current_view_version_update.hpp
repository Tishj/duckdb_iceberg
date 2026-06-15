
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

class SetCurrentViewVersionUpdateBuilder;

class SetCurrentViewVersionUpdate {
public:
	SetCurrentViewVersionUpdate(const SetCurrentViewVersionUpdate &) = delete;
	SetCurrentViewVersionUpdate &operator=(const SetCurrentViewVersionUpdate &) = delete;
	SetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate &&) = default;
	SetCurrentViewVersionUpdate &operator=(SetCurrentViewVersionUpdate &&) = delete;

private:
	friend class SetCurrentViewVersionUpdateBuilder;
	SetCurrentViewVersionUpdate(BaseUpdate base_update_p, int32_t view_version_id_p);

public:
	// Deserialization
	static SetCurrentViewVersionUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<SetCurrentViewVersionUpdate> &result);
	string Validate() const;

	// Copy
	SetCurrentViewVersionUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int32_t view_version_id;
};

class SetCurrentViewVersionUpdateBuilder {
public:
	SetCurrentViewVersionUpdateBuilder();
	SetCurrentViewVersionUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetCurrentViewVersionUpdateBuilder &SetViewVersionId(int32_t value);
	string TryBuild(optional<SetCurrentViewVersionUpdate> &result);
	SetCurrentViewVersionUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int32_t> view_version_id_;
	bool has_view_version_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
