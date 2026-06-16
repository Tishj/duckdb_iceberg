
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

class AssertCreateBuilder;

class AssertCreate {
public:
	AssertCreate(const AssertCreate &);
	AssertCreate &operator=(const AssertCreate &) = delete;
	AssertCreate(AssertCreate &&);
	AssertCreate &operator=(AssertCreate &&) = delete;

private:
	friend class AssertCreateBuilder;
	AssertCreate(TableRequirementType type_p);

public:
	// Deserialization
	static AssertCreate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, AssertCreateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	AssertCreate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
};

class AssertCreateBuilder {
public:
	AssertCreateBuilder();
	AssertCreateBuilder &SetType(TableRequirementType value);
	optional<string> TryBuild(optional<AssertCreate> &result);
	AssertCreate Build();

private:
	optional<TableRequirementType> type_;
	bool has_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
