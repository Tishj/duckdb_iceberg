
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

class SetLocationUpdateBuilder;

class SetLocationUpdate {
public:
	SetLocationUpdate(const SetLocationUpdate &);
	SetLocationUpdate &operator=(const SetLocationUpdate &) = delete;
	SetLocationUpdate(SetLocationUpdate &&);
	SetLocationUpdate &operator=(SetLocationUpdate &&) = delete;

private:
	friend class SetLocationUpdateBuilder;
	SetLocationUpdate(BaseUpdate base_update_p, string location_p);

public:
	// Deserialization
	static SetLocationUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SetLocationUpdateBuilder &builder);
	string Validate() const;

	// Copy
	SetLocationUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const string location;
};

class SetLocationUpdateBuilder {
public:
	SetLocationUpdateBuilder();
	SetLocationUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetLocationUpdateBuilder &SetLocation(string value);
	string TryBuild(optional<SetLocationUpdate> &result);
	SetLocationUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<string> location_;
	bool has_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
