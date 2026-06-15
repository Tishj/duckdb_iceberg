
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/expression_type.hpp"
#include "rest_catalog/objects/term.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class UnaryExpressionBuilder;

class UnaryExpression {
public:
	UnaryExpression(const UnaryExpression &) = delete;
	UnaryExpression &operator=(const UnaryExpression &) = delete;
	UnaryExpression(UnaryExpression &&) = default;
	UnaryExpression &operator=(UnaryExpression &&) = delete;

private:
	friend class UnaryExpressionBuilder;
	UnaryExpression(ExpressionType type_p, Term term_p);

public:
	// Deserialization
	static UnaryExpression FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<UnaryExpression> &result);
	string Validate() const;

	// Copy
	UnaryExpression Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
	Term term;
};

class UnaryExpressionBuilder {
public:
	UnaryExpressionBuilder();
	UnaryExpressionBuilder &SetType(ExpressionType value);
	UnaryExpressionBuilder &SetTerm(Term value);
	string TryBuild(optional<UnaryExpression> &result);
	UnaryExpression Build();

private:
	optional<ExpressionType> type_;
	optional<Term> term_;
	bool has_type_ = false;
	bool has_term_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
