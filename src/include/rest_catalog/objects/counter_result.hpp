
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

class CounterResultBuilder;

class CounterResult {
public:
	CounterResult(const CounterResult &) = delete;
	CounterResult &operator=(const CounterResult &) = delete;
	CounterResult(CounterResult &&) = default;
	CounterResult &operator=(CounterResult &&) = default;

private:
	friend class CounterResultBuilder;
	friend class GeneratedObjectAccess;
	CounterResult();

public:
	// Deserialization
	static CounterResult FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CounterResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string unit;
	int64_t value;
};

class CounterResultBuilder {
public:
	CounterResultBuilder();
	CounterResultBuilder &SetUnit(string value);
	CounterResultBuilder &SetValue(int64_t value);
	string TryBuild(CounterResult &result);
	CounterResult Build();

private:
	CounterResult result_;
	bool has_unit_ = false;
	bool has_value_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
