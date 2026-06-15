
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/expression_type.hpp"
#include "rest_catalog/objects/primitive_type_value.hpp"
#include "rest_catalog/objects/term.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SetExpression {
public:
	SetExpression();
	SetExpression(const SetExpression &) = delete;
	SetExpression &operator=(const SetExpression &) = delete;
	SetExpression(SetExpression &&) = default;
	SetExpression &operator=(SetExpression &&) = default;

public:
	// Deserialization
	static SetExpression FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SetExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
	Term term;
	vector<PrimitiveTypeValue> values;
};

class SetExpressionBuilder {
public:
	SetExpressionBuilder();
	SetExpressionBuilder &SetType(ExpressionType value);
	SetExpressionBuilder &SetTerm(Term value);
	SetExpressionBuilder &SetValues(vector<PrimitiveTypeValue> value);
	string TryBuild(SetExpression &result);
	SetExpression Build();

private:
	SetExpression result_;
	bool has_type_ = false;
	bool has_term_ = false;
	bool has_values_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
