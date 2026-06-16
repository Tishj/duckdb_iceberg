
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

class AssignUUIDUpdateBuilder;

class AssignUUIDUpdate {
public:
	AssignUUIDUpdate(const AssignUUIDUpdate &);
	AssignUUIDUpdate &operator=(const AssignUUIDUpdate &) = delete;
	AssignUUIDUpdate(AssignUUIDUpdate &&);
	AssignUUIDUpdate &operator=(AssignUUIDUpdate &&) = delete;

private:
	friend class AssignUUIDUpdateBuilder;
	AssignUUIDUpdate(BaseUpdate base_update_p, string uuid_p);

public:
	// Deserialization
	static AssignUUIDUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssignUUIDUpdateBuilder &builder);
	string Validate() const;

	// Copy
	AssignUUIDUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const string uuid;
};

class AssignUUIDUpdateBuilder {
public:
	AssignUUIDUpdateBuilder();
	AssignUUIDUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	AssignUUIDUpdateBuilder &SetUuid(string value);
	string TryBuild(optional<AssignUUIDUpdate> &result);
	AssignUUIDUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<string> uuid_;
	bool has_uuid_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
