
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/expression_type.hpp"
#include "rest_catalog/objects/primitive_type_value.hpp"
#include "rest_catalog/objects/term.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class LiteralExpressionBuilder;

class LiteralExpression {
public:
	LiteralExpression(const LiteralExpression &);
	LiteralExpression &operator=(const LiteralExpression &) = delete;
	LiteralExpression(LiteralExpression &&);
	LiteralExpression &operator=(LiteralExpression &&) = delete;

private:
	friend class LiteralExpressionBuilder;
	LiteralExpression(ExpressionType type_p, Term term_p, PrimitiveTypeValue value_p);

public:
	// Deserialization
	static LiteralExpression FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, LiteralExpressionBuilder &builder);
	optional<string> Validate() const;

	// Copy
	LiteralExpression Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const ExpressionType type;
	const Term term;
	const PrimitiveTypeValue value;
};

class LiteralExpressionBuilder {
public:
	LiteralExpressionBuilder();
	LiteralExpressionBuilder &SetType(ExpressionType value);
	LiteralExpressionBuilder &SetTerm(Term value);
	LiteralExpressionBuilder &SetValue(PrimitiveTypeValue value);
	optional<string> TryBuild(optional<LiteralExpression> &result);
	LiteralExpression Build();

private:
	optional<ExpressionType> type_;
	optional<Term> term_;
	optional<PrimitiveTypeValue> value_;
	bool has_type_ = false;
	bool has_term_ = false;
	bool has_value_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
