
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/assert_view_uuid.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ViewRequirementBuilder;

class ViewRequirement {
public:
	ViewRequirement(const ViewRequirement &) = delete;
	ViewRequirement &operator=(const ViewRequirement &) = delete;
	ViewRequirement(ViewRequirement &&) = default;
	ViewRequirement &operator=(ViewRequirement &&) = delete;

private:
	friend class ViewRequirementBuilder;
	ViewRequirement(optional<AssertViewUUID> assert_view_uuid_p);

public:
	// Deserialization
	static ViewRequirement FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<ViewRequirement> &result);
	string Validate() const;

	// Copy
	ViewRequirement Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<AssertViewUUID> assert_view_uuid;
};

class ViewRequirementBuilder {
public:
	ViewRequirementBuilder();
	ViewRequirementBuilder &SetAssertViewUuid(AssertViewUUID value);
	string TryBuild(optional<ViewRequirement> &result);
	ViewRequirement Build();

private:
	optional<AssertViewUUID> assert_view_uuid_;
};

} // namespace rest_api_objects
} // namespace duckdb
