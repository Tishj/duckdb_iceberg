
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/snapshot_reference.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SnapshotReferencesBuilder;

class SnapshotReferences {
public:
	SnapshotReferences(const SnapshotReferences &);
	SnapshotReferences &operator=(const SnapshotReferences &) = delete;
	SnapshotReferences(SnapshotReferences &&);
	SnapshotReferences &operator=(SnapshotReferences &&) = delete;

private:
	friend class SnapshotReferencesBuilder;
	SnapshotReferences(case_insensitive_map_t<SnapshotReference> additional_properties_p);

public:
	// Deserialization
	static SnapshotReferences FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, SnapshotReferencesBuilder &builder);
	optional<string> Validate() const;

	// Copy
	SnapshotReferences Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const case_insensitive_map_t<SnapshotReference> additional_properties;
};

class SnapshotReferencesBuilder {
public:
	SnapshotReferencesBuilder();
	SnapshotReferencesBuilder &SetAdditionalProperties(case_insensitive_map_t<SnapshotReference> value);
	optional<string> TryBuild(optional<SnapshotReferences> &result);
	SnapshotReferences Build();

private:
	optional<case_insensitive_map_t<SnapshotReference>> additional_properties_;
};

} // namespace rest_api_objects
} // namespace duckdb
