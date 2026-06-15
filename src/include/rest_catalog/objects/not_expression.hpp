
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

class Expression;

class NotExpressionBuilder;

class NotExpression {
public:
	NotExpression(const NotExpression &) = delete;
	NotExpression &operator=(const NotExpression &) = delete;
	NotExpression(NotExpression &&) = default;
	NotExpression &operator=(NotExpression &&) = default;

private:
	friend class NotExpressionBuilder;
	friend class GeneratedObjectAccess;
	NotExpression();

public:
	// Deserialization
	static NotExpression FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	NotExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
	unique_ptr<Expression> child;
};

class NotExpressionBuilder {
public:
	NotExpressionBuilder();
	NotExpressionBuilder &SetType(ExpressionType value);
	NotExpressionBuilder &SetChild(unique_ptr<Expression> value);
	string TryBuild(NotExpression &result);
	NotExpression Build();

private:
	NotExpression result_;
	bool has_type_ = false;
	bool has_child_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
