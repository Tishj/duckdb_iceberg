
#include "rest_catalog/objects/expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Expression::Expression(optional<TrueExpression> true_expression_p, optional<FalseExpression> false_expression_p,
                       optional<AndOrExpression> and_or_expression_p, optional<NotExpression> not_expression_p,
                       optional<SetExpression> set_expression_p, optional<LiteralExpression> literal_expression_p,
                       optional<UnaryExpression> unary_expression_p)
    : true_expression(std::move(true_expression_p)), false_expression(std::move(false_expression_p)),
      and_or_expression(std::move(and_or_expression_p)), not_expression(std::move(not_expression_p)),
      set_expression(std::move(set_expression_p)), literal_expression(std::move(literal_expression_p)),
      unary_expression(std::move(unary_expression_p)) {
}
Expression::Expression(const Expression &other)
    : true_expression((other.true_expression.has_value() ? optional<TrueExpression>((*other.true_expression).Copy())
                                                         : optional<TrueExpression>())),
      false_expression((other.false_expression.has_value() ? optional<FalseExpression>((*other.false_expression).Copy())
                                                           : optional<FalseExpression>())),
      and_or_expression((other.and_or_expression.has_value()
                             ? optional<AndOrExpression>((*other.and_or_expression).Copy())
                             : optional<AndOrExpression>())),
      not_expression((other.not_expression.has_value() ? optional<NotExpression>((*other.not_expression).Copy())
                                                       : optional<NotExpression>())),
      set_expression((other.set_expression.has_value() ? optional<SetExpression>((*other.set_expression).Copy())
                                                       : optional<SetExpression>())),
      literal_expression((other.literal_expression.has_value()
                              ? optional<LiteralExpression>((*other.literal_expression).Copy())
                              : optional<LiteralExpression>())),
      unary_expression((other.unary_expression.has_value() ? optional<UnaryExpression>((*other.unary_expression).Copy())
                                                           : optional<UnaryExpression>())) {
}
Expression::Expression(Expression &&other) : Expression(static_cast<const Expression &>(other)) {
}

ExpressionBuilder::ExpressionBuilder() {
}

ExpressionBuilder &ExpressionBuilder::SetTrueExpression(TrueExpression value) {
	true_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetFalseExpression(FalseExpression value) {
	false_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetAndOrExpression(AndOrExpression value) {
	and_or_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetNotExpression(NotExpression value) {
	not_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetSetExpression(SetExpression value) {
	set_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetLiteralExpression(LiteralExpression value) {
	literal_expression_.emplace(std::move(value));
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetUnaryExpression(UnaryExpression value) {
	unary_expression_.emplace(std::move(value));
	return *this;
}

Expression ExpressionBuilder::Build() {
	auto result = Expression(std::move(true_expression_), std::move(false_expression_), std::move(and_or_expression_),
	                         std::move(not_expression_), std::move(set_expression_), std::move(literal_expression_),
	                         std::move(unary_expression_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> ExpressionBuilder::TryBuild(optional<Expression> &result) {
	auto built = Expression(std::move(true_expression_), std::move(false_expression_), std::move(and_or_expression_),
	                        std::move(not_expression_), std::move(set_expression_), std::move(literal_expression_),
	                        std::move(unary_expression_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> Expression::TryFromJSON(yyjson_val *obj, ExpressionBuilder &builder) {
	try {
		do {
			try {
				builder.SetTrueExpression(TrueExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetFalseExpression(FalseExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetAndOrExpression(AndOrExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetNotExpression(NotExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetSetExpression(SetExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetLiteralExpression(LiteralExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetUnaryExpression(UnaryExpression::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			throw InvalidInputException("Expression failed to parse, none of the oneOf candidates matched");
		} while (false);
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Expression Expression::FromJSON(yyjson_val *obj) {
	ExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

Expression Expression::Copy() const {
	return Expression(*this);
}

optional<string> Expression::Validate() const {
	optional<string> error;
	int matched_one_of_variants = 0;
	if (true_expression.has_value()) {
		matched_one_of_variants++;
		error = true_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (false_expression.has_value()) {
		matched_one_of_variants++;
		error = false_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (and_or_expression.has_value()) {
		matched_one_of_variants++;
		error = and_or_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (not_expression.has_value()) {
		matched_one_of_variants++;
		error = not_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (set_expression.has_value()) {
		matched_one_of_variants++;
		error = set_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (literal_expression.has_value()) {
		matched_one_of_variants++;
		error = literal_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (unary_expression.has_value()) {
		matched_one_of_variants++;
		error = unary_expression->Validate();
		if (error) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "Expression must have exactly one oneOf variant set";
	}
	return nullopt;
}

void Expression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (true_expression.has_value()) {
		true_expression->PopulateJSON(doc, obj);
	} else if (false_expression.has_value()) {
		false_expression->PopulateJSON(doc, obj);
	} else if (and_or_expression.has_value()) {
		and_or_expression->PopulateJSON(doc, obj);
	} else if (not_expression.has_value()) {
		not_expression->PopulateJSON(doc, obj);
	} else if (set_expression.has_value()) {
		set_expression->PopulateJSON(doc, obj);
	} else if (literal_expression.has_value()) {
		yyjson_mut_val *literal_expression_obj = literal_expression->ToJSON(doc);
		if (!yyjson_mut_is_obj(literal_expression_obj)) {
			throw InternalException("PopulateJSON requires an object-like JSON value");
		}
		{
			size_t idx, max;
			yyjson_mut_val *key, *val;
			yyjson_mut_obj_foreach(literal_expression_obj, idx, max, key, val) {
				yyjson_mut_obj_add(obj, key, val);
			}
		}
	} else if (unary_expression.has_value()) {
		yyjson_mut_val *unary_expression_obj = unary_expression->ToJSON(doc);
		if (!yyjson_mut_is_obj(unary_expression_obj)) {
			throw InternalException("PopulateJSON requires an object-like JSON value");
		}
		{
			size_t idx, max;
			yyjson_mut_val *key, *val;
			yyjson_mut_obj_foreach(unary_expression_obj, idx, max, key, val) {
				yyjson_mut_obj_add(obj, key, val);
			}
		}
	}
}

yyjson_mut_val *Expression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
