
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
	CounterResult(const CounterResult &);
	CounterResult &operator=(const CounterResult &) = delete;
	CounterResult(CounterResult &&);
	CounterResult &operator=(CounterResult &&) = delete;

private:
	friend class CounterResultBuilder;
	CounterResult(string unit_p, int64_t value_p);

public:
	// Deserialization
	static CounterResult FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, CounterResultBuilder &builder);
	optional<string> Validate() const;

	// Copy
	CounterResult Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string unit;
	const int64_t value;
};

class CounterResultBuilder {
public:
	CounterResultBuilder();
	CounterResultBuilder &SetUnit(string value);
	CounterResultBuilder &SetValue(int64_t value);
	optional<string> TryBuild(optional<CounterResult> &result);
	CounterResult Build();

private:
	optional<string> unit_;
	optional<int64_t> value_;
	bool has_unit_ = false;
	bool has_value_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
