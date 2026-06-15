
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

class AndOrExpressionBuilder;

class AndOrExpression {
public:
	AndOrExpression(const AndOrExpression &) = delete;
	AndOrExpression &operator=(const AndOrExpression &) = delete;
	AndOrExpression(AndOrExpression &&) = default;
	AndOrExpression &operator=(AndOrExpression &&) = delete;

private:
	friend class AndOrExpressionBuilder;
	AndOrExpression(ExpressionType type_p, unique_ptr<Expression> left_p, unique_ptr<Expression> right_p);

public:
	// Deserialization
	static AndOrExpression FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<AndOrExpression> &result);
	string Validate() const;

	// Copy
	AndOrExpression Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	ExpressionType type;
	unique_ptr<Expression> left;
	unique_ptr<Expression> right;
};

class AndOrExpressionBuilder {
public:
	AndOrExpressionBuilder();
	AndOrExpressionBuilder &SetType(ExpressionType value);
	AndOrExpressionBuilder &SetLeft(unique_ptr<Expression> value);
	AndOrExpressionBuilder &SetRight(unique_ptr<Expression> value);
	string TryBuild(optional<AndOrExpression> &result);
	AndOrExpression Build();

private:
	optional<ExpressionType> type_;
	unique_ptr<Expression> left_;
	unique_ptr<Expression> right_;
	bool has_type_ = false;
	bool has_left_ = false;
	bool has_right_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
