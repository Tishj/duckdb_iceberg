
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

class ErrorModelBuilder;

class ErrorModel {
public:
	ErrorModel(const ErrorModel &) = delete;
	ErrorModel &operator=(const ErrorModel &) = delete;
	ErrorModel(ErrorModel &&) = default;
	ErrorModel &operator=(ErrorModel &&) = default;

private:
	friend class ErrorModelBuilder;
	friend class GeneratedObjectAccess;
	ErrorModel();

public:
	// Deserialization
	static ErrorModel FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	ErrorModel Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string message;
	string type;
	int32_t code;
	optional<vector<string>> stack;
};

class ErrorModelBuilder {
public:
	ErrorModelBuilder();
	ErrorModelBuilder &SetMessage(string value);
	ErrorModelBuilder &SetType(string value);
	ErrorModelBuilder &SetCode(int32_t value);
	ErrorModelBuilder &SetStack(vector<string> value);
	string TryBuild(ErrorModel &result);
	ErrorModel Build();

private:
	ErrorModel result_;
	bool has_message_ = false;
	bool has_type_ = false;
	bool has_code_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
