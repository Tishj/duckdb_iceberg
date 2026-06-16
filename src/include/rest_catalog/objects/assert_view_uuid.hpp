
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

class AssertViewUUIDBuilder;

class AssertViewUUID {
public:
	AssertViewUUID(const AssertViewUUID &) = delete;
	AssertViewUUID &operator=(const AssertViewUUID &) = delete;
	AssertViewUUID(AssertViewUUID &&) = default;
	AssertViewUUID &operator=(AssertViewUUID &&) = delete;

private:
	friend class AssertViewUUIDBuilder;
	AssertViewUUID(string type_p, string uuid_p);

public:
	// Deserialization
	static AssertViewUUID FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertViewUUIDBuilder &builder);
	string Validate() const;

	// Copy
	AssertViewUUID Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	string uuid;
};

class AssertViewUUIDBuilder {
public:
	AssertViewUUIDBuilder();
	AssertViewUUIDBuilder &SetType(string value);
	AssertViewUUIDBuilder &SetUuid(string value);
	string TryBuild(optional<AssertViewUUID> &result);
	AssertViewUUID Build();

private:
	optional<string> type_;
	optional<string> uuid_;
	bool has_type_ = false;
	bool has_uuid_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
