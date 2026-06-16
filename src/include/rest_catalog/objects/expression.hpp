
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/and_or_expression.hpp"
#include "rest_catalog/objects/false_expression.hpp"
#include "rest_catalog/objects/literal_expression.hpp"
#include "rest_catalog/objects/not_expression.hpp"
#include "rest_catalog/objects/set_expression.hpp"
#include "rest_catalog/objects/true_expression.hpp"
#include "rest_catalog/objects/unary_expression.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ExpressionBuilder;

class Expression {
public:
	Expression(const Expression &);
	Expression &operator=(const Expression &) = delete;
	Expression(Expression &&);
	Expression &operator=(Expression &&) = delete;

private:
	friend class ExpressionBuilder;
	Expression(optional<TrueExpression> true_expression_p, optional<FalseExpression> false_expression_p,
	           optional<AndOrExpression> and_or_expression_p, optional<NotExpression> not_expression_p,
	           optional<SetExpression> set_expression_p, optional<LiteralExpression> literal_expression_p,
	           optional<UnaryExpression> unary_expression_p);

public:
	// Deserialization
	static Expression FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, ExpressionBuilder &builder);
	optional<string> Validate() const;

	// Copy
	Expression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<TrueExpression> true_expression;
	const optional<FalseExpression> false_expression;
	const optional<AndOrExpression> and_or_expression;
	const optional<NotExpression> not_expression;
	const optional<SetExpression> set_expression;
	const optional<LiteralExpression> literal_expression;
	const optional<UnaryExpression> unary_expression;
};

class ExpressionBuilder {
public:
	ExpressionBuilder();
	ExpressionBuilder &SetTrueExpression(TrueExpression value);
	ExpressionBuilder &SetFalseExpression(FalseExpression value);
	ExpressionBuilder &SetAndOrExpression(AndOrExpression value);
	ExpressionBuilder &SetNotExpression(NotExpression value);
	ExpressionBuilder &SetSetExpression(SetExpression value);
	ExpressionBuilder &SetLiteralExpression(LiteralExpression value);
	ExpressionBuilder &SetUnaryExpression(UnaryExpression value);
	optional<string> TryBuild(optional<Expression> &result);
	Expression Build();

private:
	optional<TrueExpression> true_expression_;
	optional<FalseExpression> false_expression_;
	optional<AndOrExpression> and_or_expression_;
	optional<NotExpression> not_expression_;
	optional<SetExpression> set_expression_;
	optional<LiteralExpression> literal_expression_;
	optional<UnaryExpression> unary_expression_;
};

} // namespace rest_api_objects
} // namespace duckdb
