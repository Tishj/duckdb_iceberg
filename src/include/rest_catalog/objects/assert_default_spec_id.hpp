
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

class AssertDefaultSpecIdBuilder;

class AssertDefaultSpecId {
public:
	AssertDefaultSpecId(const AssertDefaultSpecId &);
	AssertDefaultSpecId &operator=(const AssertDefaultSpecId &) = delete;
	AssertDefaultSpecId(AssertDefaultSpecId &&);
	AssertDefaultSpecId &operator=(AssertDefaultSpecId &&) = delete;

private:
	friend class AssertDefaultSpecIdBuilder;
	AssertDefaultSpecId(TableRequirementType type_p, int32_t default_spec_id_p);

public:
	// Deserialization
	static AssertDefaultSpecId FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, AssertDefaultSpecIdBuilder &builder);
	string Validate() const;

	// Copy
	AssertDefaultSpecId Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const TableRequirementType type;
	const int32_t default_spec_id;
};

class AssertDefaultSpecIdBuilder {
public:
	AssertDefaultSpecIdBuilder();
	AssertDefaultSpecIdBuilder &SetType(TableRequirementType value);
	AssertDefaultSpecIdBuilder &SetDefaultSpecId(int32_t value);
	string TryBuild(optional<AssertDefaultSpecId> &result);
	AssertDefaultSpecId Build();

private:
	optional<TableRequirementType> type_;
	optional<int32_t> default_spec_id_;
	bool has_type_ = false;
	bool has_default_spec_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
