
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

class SetExpressionBuilder;

class SetExpression {
public:
	SetExpression(const SetExpression &);
	SetExpression &operator=(const SetExpression &) = delete;
	SetExpression(SetExpression &&);
	SetExpression &operator=(SetExpression &&) = delete;

private:
	friend class SetExpressionBuilder;
	SetExpression(ExpressionType type_p, Term term_p, vector<PrimitiveTypeValue> values_p);

public:
	// Deserialization
	static SetExpression FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, SetExpressionBuilder &builder);
	optional<string> Validate() const;

	// Copy
	SetExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const ExpressionType type;
	const Term term;
	const vector<PrimitiveTypeValue> values;
};

class SetExpressionBuilder {
public:
	SetExpressionBuilder();
	SetExpressionBuilder &SetType(ExpressionType value);
	SetExpressionBuilder &SetTerm(Term value);
	SetExpressionBuilder &SetValues(vector<PrimitiveTypeValue> value);
	optional<string> TryBuild(optional<SetExpression> &result);
	SetExpression Build();

private:
	optional<ExpressionType> type_;
	optional<Term> term_;
	optional<vector<PrimitiveTypeValue>> values_;
	bool has_type_ = false;
	bool has_term_ = false;
	bool has_values_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
