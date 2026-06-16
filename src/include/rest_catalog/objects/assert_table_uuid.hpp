
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/table_requirement_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class AssertTableUUIDBuilder;

class AssertTableUUID {
public:
	AssertTableUUID(const AssertTableUUID &);
	AssertTableUUID &operator=(const AssertTableUUID &) = delete;
	AssertTableUUID(AssertTableUUID &&);
	AssertTableUUID &operator=(AssertTableUUID &&) = delete;

private:
	friend class AssertTableUUIDBuilder;
	AssertTableUUID(TableRequirementType type_p, string uuid_p);

public:
	// Deserialization
	static AssertTableUUID FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, AssertTableUUIDBuilder &builder);
	optional<string> Validate() const;

	// Copy
	AssertTableUUID Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
	const string uuid;
};

class AssertTableUUIDBuilder {
public:
	AssertTableUUIDBuilder();
	AssertTableUUIDBuilder &SetType(TableRequirementType value);
	AssertTableUUIDBuilder &SetUuid(string value);
	optional<string> TryBuild(optional<AssertTableUUID> &result);
	AssertTableUUID Build();

private:
	optional<TableRequirementType> type_;
	optional<string> uuid_;
	bool has_type_ = false;
	bool has_uuid_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
