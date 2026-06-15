
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class BlobMetadata {
public:
	BlobMetadata();
	BlobMetadata(const BlobMetadata &) = delete;
	BlobMetadata &operator=(const BlobMetadata &) = delete;
	BlobMetadata(BlobMetadata &&) = default;
	BlobMetadata &operator=(BlobMetadata &&) = default;

public:
	// Deserialization
	static BlobMetadata FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(BlobMetadata &result);
	BlobMetadata Build();

private:
	BlobMetadata result_;
	bool has_type_ = false;
	bool has_snapshot_id_ = false;
	bool has_sequence_number_ = false;
	bool has_fields_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
