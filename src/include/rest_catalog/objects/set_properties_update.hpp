
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetPropertiesUpdate {
public:
	SetPropertiesUpdate();
	SetPropertiesUpdate(const SetPropertiesUpdate &) = delete;
	SetPropertiesUpdate &operator=(const SetPropertiesUpdate &) = delete;
	SetPropertiesUpdate(SetPropertiesUpdate &&) = default;
	SetPropertiesUpdate &operator=(SetPropertiesUpdate &&) = default;

public:
	// Deserialization
	static SetPropertiesUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetPropertiesUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	case_insensitive_map_t<string> updates;
};

class SetPropertiesUpdateBuilder {
public:
	SetPropertiesUpdateBuilder();
	SetPropertiesUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetPropertiesUpdateBuilder &SetUpdates(case_insensitive_map_t<string> value);
	string TryBuild(SetPropertiesUpdate &result);
	SetPropertiesUpdate Build();

private:
	SetPropertiesUpdate result_;
	bool has_updates_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
