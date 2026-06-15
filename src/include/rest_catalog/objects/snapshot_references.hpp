
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
	SnapshotReferences(const SnapshotReferences &) = delete;
	SnapshotReferences &operator=(const SnapshotReferences &) = delete;
	SnapshotReferences(SnapshotReferences &&) = default;
	SnapshotReferences &operator=(SnapshotReferences &&) = default;

private:
	friend class SnapshotReferencesBuilder;
	friend class GeneratedObjectAccess;
	SnapshotReferences();

public:
	// Deserialization
	static SnapshotReferences FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SnapshotReferences Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	case_insensitive_map_t<SnapshotReference> additional_properties;
};

class SnapshotReferencesBuilder {
public:
	SnapshotReferencesBuilder();
	SnapshotReferencesBuilder &SetAdditionalProperties(case_insensitive_map_t<SnapshotReference> value);
	string TryBuild(SnapshotReferences &result);
	SnapshotReferences Build();

private:
	SnapshotReferences result_;
};

} // namespace rest_api_objects
} // namespace duckdb
