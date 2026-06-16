
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

class BlobMetadataBuilder;

class BlobMetadata {
public:
	BlobMetadata(const BlobMetadata &) = delete;
	BlobMetadata &operator=(const BlobMetadata &) = delete;
	BlobMetadata(BlobMetadata &&) = default;
	BlobMetadata &operator=(BlobMetadata &&) = delete;

private:
	friend class BlobMetadataBuilder;
	BlobMetadata(string type_p, int64_t snapshot_id_p, int64_t sequence_number_p, vector<int32_t> fields_p,
	             optional<case_insensitive_map_t<string>> properties_p);

public:
	// Deserialization
	static BlobMetadata FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, BlobMetadataBuilder &builder);
	string Validate() const;

	// Copy
	BlobMetadata Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	int64_t snapshot_id;
	int64_t sequence_number;
	vector<int32_t> fields;
	optional<case_insensitive_map_t<string>> properties;
};

class BlobMetadataBuilder {
public:
	BlobMetadataBuilder();
	BlobMetadataBuilder &SetType(string value);
	BlobMetadataBuilder &SetSnapshotId(int64_t value);
	BlobMetadataBuilder &SetSequenceNumber(int64_t value);
	BlobMetadataBuilder &SetFields(vector<int32_t> value);
	BlobMetadataBuilder &SetProperties(case_insensitive_map_t<string> value);
	string TryBuild(optional<BlobMetadata> &result);
	BlobMetadata Build();

private:
	optional<string> type_;
	optional<int64_t> snapshot_id_;
	optional<int64_t> sequence_number_;
	optional<vector<int32_t>> fields_;
	optional<case_insensitive_map_t<string>> properties_;
	bool has_type_ = false;
	bool has_snapshot_id_ = false;
	bool has_sequence_number_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
