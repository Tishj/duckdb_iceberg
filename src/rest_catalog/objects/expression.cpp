
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
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ExpressionBuilder::TryBuild(optional<Expression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string Expression::TryFromJSON(yyjson_val *obj, ExpressionBuilder &builder) {
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
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Expression Expression::FromJSON(yyjson_val *obj) {
	ExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

Expression Expression::Copy() const {
	ExpressionBuilder builder;
	optional<TrueExpression> true_expression_tmp;
	if (true_expression.has_value()) {
		true_expression_tmp.emplace((*true_expression).Copy());
	}
	if (true_expression_tmp.has_value()) {
		builder.SetTrueExpression(std::move(*true_expression_tmp));
	}
	optional<FalseExpression> false_expression_tmp;
	if (false_expression.has_value()) {
		false_expression_tmp.emplace((*false_expression).Copy());
	}
	if (false_expression_tmp.has_value()) {
		builder.SetFalseExpression(std::move(*false_expression_tmp));
	}
	optional<AndOrExpression> and_or_expression_tmp;
	if (and_or_expression.has_value()) {
		and_or_expression_tmp.emplace((*and_or_expression).Copy());
	}
	if (and_or_expression_tmp.has_value()) {
		builder.SetAndOrExpression(std::move(*and_or_expression_tmp));
	}
	optional<NotExpression> not_expression_tmp;
	if (not_expression.has_value()) {
		not_expression_tmp.emplace((*not_expression).Copy());
	}
	if (not_expression_tmp.has_value()) {
		builder.SetNotExpression(std::move(*not_expression_tmp));
	}
	optional<SetExpression> set_expression_tmp;
	if (set_expression.has_value()) {
		set_expression_tmp.emplace((*set_expression).Copy());
	}
	if (set_expression_tmp.has_value()) {
		builder.SetSetExpression(std::move(*set_expression_tmp));
	}
	optional<LiteralExpression> literal_expression_tmp;
	if (literal_expression.has_value()) {
		literal_expression_tmp.emplace((*literal_expression).Copy());
	}
	if (literal_expression_tmp.has_value()) {
		builder.SetLiteralExpression(std::move(*literal_expression_tmp));
	}
	optional<UnaryExpression> unary_expression_tmp;
	if (unary_expression.has_value()) {
		unary_expression_tmp.emplace((*unary_expression).Copy());
	}
	if (unary_expression_tmp.has_value()) {
		builder.SetUnaryExpression(std::move(*unary_expression_tmp));
	}
	return builder.Build();
}

string Expression::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (true_expression.has_value()) {
		matched_one_of_variants++;
		error = true_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (false_expression.has_value()) {
		matched_one_of_variants++;
		error = false_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (and_or_expression.has_value()) {
		matched_one_of_variants++;
		error = and_or_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (not_expression.has_value()) {
		matched_one_of_variants++;
		error = not_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (set_expression.has_value()) {
		matched_one_of_variants++;
		error = set_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (literal_expression.has_value()) {
		matched_one_of_variants++;
		error = literal_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (unary_expression.has_value()) {
		matched_one_of_variants++;
		error = unary_expression->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "Expression must have exactly one oneOf variant set";
	}
	return "";
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
