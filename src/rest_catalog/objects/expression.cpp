
#include "rest_catalog/objects/expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Expression::Expression() {
}

ExpressionBuilder::ExpressionBuilder() {
}

ExpressionBuilder &ExpressionBuilder::SetTrueExpression(TrueExpression value) {
	result_.true_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetFalseExpression(FalseExpression value) {
	result_.false_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetAndOrExpression(AndOrExpression value) {
	result_.and_or_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetNotExpression(NotExpression value) {
	result_.not_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetSetExpression(SetExpression value) {
	result_.set_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetLiteralExpression(LiteralExpression value) {
	result_.literal_expression = std::move(value);
	return *this;
}

ExpressionBuilder &ExpressionBuilder::SetUnaryExpression(UnaryExpression value) {
	result_.unary_expression = std::move(value);
	return *this;
}

string ExpressionBuilder::TryBuild(Expression &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Expression ExpressionBuilder::Build() {
	Expression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Expression Expression::FromJSON(yyjson_val *obj) {
	Expression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Expression Expression::Copy() const {
	Expression res;
	if (true_expression.has_value()) {
		res.true_expression.emplace();
		(*res.true_expression) = (*true_expression).Copy();
	}
	if (false_expression.has_value()) {
		res.false_expression.emplace();
		(*res.false_expression) = (*false_expression).Copy();
	}
	if (and_or_expression.has_value()) {
		res.and_or_expression.emplace();
		(*res.and_or_expression) = (*and_or_expression).Copy();
	}
	if (not_expression.has_value()) {
		res.not_expression.emplace();
		(*res.not_expression) = (*not_expression).Copy();
	}
	if (set_expression.has_value()) {
		res.set_expression.emplace();
		(*res.set_expression) = (*set_expression).Copy();
	}
	if (literal_expression.has_value()) {
		res.literal_expression.emplace();
		(*res.literal_expression) = (*literal_expression).Copy();
	}
	if (unary_expression.has_value()) {
		res.unary_expression.emplace();
		(*res.unary_expression) = (*unary_expression).Copy();
	}
	return res;
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

string Expression::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		true_expression.emplace();
		error = true_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			true_expression = nullopt;
		}
		false_expression.emplace();
		error = false_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			false_expression = nullopt;
		}
		and_or_expression.emplace();
		error = and_or_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			and_or_expression = nullopt;
		}
		not_expression.emplace();
		error = not_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			not_expression = nullopt;
		}
		set_expression.emplace();
		error = set_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			set_expression = nullopt;
		}
		literal_expression.emplace();
		error = literal_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			literal_expression = nullopt;
		}
		unary_expression.emplace();
		error = unary_expression->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			unary_expression = nullopt;
		}
		return "Expression failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
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
