
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
	ErrorModel(const ErrorModel &);
	ErrorModel &operator=(const ErrorModel &) = delete;
	ErrorModel(ErrorModel &&);
	ErrorModel &operator=(ErrorModel &&) = delete;

private:
	friend class ErrorModelBuilder;
	ErrorModel(string message_p, string type_p, int32_t code_p, optional<vector<string>> stack_p);

public:
	// Deserialization
	static ErrorModel FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, ErrorModelBuilder &builder);
	optional<string> Validate() const;

	// Copy
	ErrorModel Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string message;
	const string type;
	const int32_t code;
	const optional<vector<string>> stack;
};

class ErrorModelBuilder {
public:
	ErrorModelBuilder();
	ErrorModelBuilder &SetMessage(string value);
	ErrorModelBuilder &SetType(string value);
	ErrorModelBuilder &SetCode(int32_t value);
	ErrorModelBuilder &SetStack(vector<string> value);
	optional<string> TryBuild(optional<ErrorModel> &result);
	ErrorModel Build();

private:
	optional<string> message_;
	optional<string> type_;
	optional<int32_t> code_;
	optional<vector<string>> stack_;
	bool has_message_ = false;
	bool has_type_ = false;
	bool has_code_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
