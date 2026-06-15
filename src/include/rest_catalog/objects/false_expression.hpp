
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/expression_type.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class FalseExpressionBuilder;

class FalseExpression {
public:
	FalseExpression(const FalseExpression &) = delete;
	FalseExpression &operator=(const FalseExpression &) = delete;
	FalseExpression(FalseExpression &&) = default;
	FalseExpression &operator=(FalseExpression &&) = delete;

private:
	friend class FalseExpressionBuilder;
	FalseExpression(ExpressionType type_p);

public:
	// Deserialization
	static FalseExpression FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<FalseExpression> &result);
	string Validate() const;

	// Copy
	FalseExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
};

class FalseExpressionBuilder {
public:
	FalseExpressionBuilder();
	FalseExpressionBuilder &SetType(ExpressionType value);
	string TryBuild(optional<FalseExpression> &result);
	FalseExpression Build();

private:
	optional<ExpressionType> type_;
	bool has_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
