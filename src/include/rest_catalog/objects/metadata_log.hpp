
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

class MetadataLog {
public:
	MetadataLog(const MetadataLog &) = delete;
	MetadataLog &operator=(const MetadataLog &) = delete;
	MetadataLog(MetadataLog &&) = default;
	MetadataLog &operator=(MetadataLog &&) = default;
	MetadataLog();

public:
	class Object4Builder;

	class Object4 {
	public:
		Object4(const Object4 &) = delete;
		Object4 &operator=(const Object4 &) = delete;
		Object4(Object4 &&) = default;
		Object4 &operator=(Object4 &&) = default;

	private:
		friend class Object4Builder;
		friend class GeneratedObjectAccess;
		Object4();

	public:
		// Deserialization
		static Object4 FromJSON(yyjson_val *obj);
		string TryFromJSON(yyjson_val *obj);
		string Validate() const;

		// Copy
		Object4 Copy() const;

		// Serialization
		void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
		yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

	public:
		string metadata_file;
		int64_t timestamp_ms;
	};

	class Object4Builder {
	public:
		Object4Builder();
		Object4Builder &SetMetadataFile(string value);
		Object4Builder &SetTimestampMs(int64_t value);
		string TryBuild(Object4 &result);
		Object4 Build();

	private:
		Object4 result_;
		bool has_metadata_file_ = false;
		bool has_timestamp_ms_ = false;
	};

public:
	// Deserialization
	static MetadataLog FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	MetadataLog Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	vector<Object4> value;
};

} // namespace rest_api_objects
} // namespace duckdb
