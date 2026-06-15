
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

class SetLocationUpdate {
public:
	SetLocationUpdate();
	SetLocationUpdate(const SetLocationUpdate &) = delete;
	SetLocationUpdate &operator=(const SetLocationUpdate &) = delete;
	SetLocationUpdate(SetLocationUpdate &&) = default;
	SetLocationUpdate &operator=(SetLocationUpdate &&) = default;

public:
	// Deserialization
	static SetLocationUpdate FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetLocationUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	string location;
};

class SetLocationUpdateBuilder {
public:
	SetLocationUpdateBuilder();
	SetLocationUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetLocationUpdateBuilder &SetLocation(string value);
	string TryBuild(SetLocationUpdate &result);
	SetLocationUpdate Build();

private:
	SetLocationUpdate result_;
	bool has_location_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
