
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

class SetPropertiesUpdateBuilder;

class SetPropertiesUpdate {
public:
	SetPropertiesUpdate(const SetPropertiesUpdate &);
	SetPropertiesUpdate &operator=(const SetPropertiesUpdate &) = delete;
	SetPropertiesUpdate(SetPropertiesUpdate &&);
	SetPropertiesUpdate &operator=(SetPropertiesUpdate &&) = delete;

private:
	friend class SetPropertiesUpdateBuilder;
	SetPropertiesUpdate(BaseUpdate base_update_p, case_insensitive_map_t<string> updates_p);

public:
	// Deserialization
	static SetPropertiesUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, SetPropertiesUpdateBuilder &builder);
	string Validate() const;

	// Copy
	SetPropertiesUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const case_insensitive_map_t<string> updates;
};

class SetPropertiesUpdateBuilder {
public:
	SetPropertiesUpdateBuilder();
	SetPropertiesUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetPropertiesUpdateBuilder &SetUpdates(case_insensitive_map_t<string> value);
	string TryBuild(optional<SetPropertiesUpdate> &result);
	SetPropertiesUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<case_insensitive_map_t<string>> updates_;
	bool has_updates_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
