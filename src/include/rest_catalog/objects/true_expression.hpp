
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

class TrueExpressionBuilder;

class TrueExpression {
public:
	TrueExpression(const TrueExpression &) = delete;
	TrueExpression &operator=(const TrueExpression &) = delete;
	TrueExpression(TrueExpression &&) = default;
	TrueExpression &operator=(TrueExpression &&) = delete;

private:
	friend class TrueExpressionBuilder;
	TrueExpression(ExpressionType type_p);

public:
	// Deserialization
	static TrueExpression FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<TrueExpression> &result);
	string Validate() const;

	// Copy
	TrueExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
};

class TrueExpressionBuilder {
public:
	TrueExpressionBuilder();
	TrueExpressionBuilder &SetType(ExpressionType value);
	string TryBuild(optional<TrueExpression> &result);
	TrueExpression Build();

private:
	optional<ExpressionType> type_;
	bool has_type_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
