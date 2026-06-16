
#include "rest_catalog/objects/literal_expression.hpp"

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

LiteralExpression::LiteralExpression(ExpressionType type_p, Term term_p, PrimitiveTypeValue value_p)
    : type(std::move(type_p)), term(std::move(term_p)), value(std::move(value_p)) {
}

LiteralExpressionBuilder::LiteralExpressionBuilder() {
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetTerm(Term value) {
	term_.emplace(std::move(value));
	has_term_ = true;
	return *this;
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetValue(PrimitiveTypeValue value) {
	value_.emplace(std::move(value));
	has_value_ = true;
	return *this;
}

LiteralExpression LiteralExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("LiteralExpression required property 'type' is missing");
	}
	if (!has_term_) {
		throw InvalidInputException("LiteralExpression required property 'term' is missing");
	}
	if (!has_value_) {
		throw InvalidInputException("LiteralExpression required property 'value' is missing");
	}
	auto result = LiteralExpression(std::move(*type_), std::move(*term_), std::move(*value_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string LiteralExpressionBuilder::TryBuild(optional<LiteralExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string LiteralExpression::TryFromJSON(yyjson_val *obj, LiteralExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("LiteralExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		auto term_val = yyjson_obj_get(obj, "term");
		if (!term_val) {
			throw InvalidInputException("LiteralExpression required property 'term' is missing");
		} else {
			builder.SetTerm(Term::FromJSON(term_val));
		}
		auto value_val = yyjson_obj_get(obj, "value");
		if (!value_val) {
			throw InvalidInputException("LiteralExpression required property 'value' is missing");
		} else {
			builder.SetValue(PrimitiveTypeValue::FromJSON(value_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LiteralExpression LiteralExpression::FromJSON(yyjson_val *obj) {
	LiteralExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

LiteralExpression LiteralExpression::Copy() const {
	LiteralExpressionBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	auto term_tmp = term.Copy();
	builder.SetTerm(std::move(term_tmp));
	auto value_tmp = value.Copy();
	builder.SetValue(std::move(value_tmp));
	return builder.Build();
}

string LiteralExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "lt") && !StringUtil::CIEquals(type.value, "lt-eq") &&
	    !StringUtil::CIEquals(type.value, "gt") && !StringUtil::CIEquals(type.value, "gt-eq") &&
	    !StringUtil::CIEquals(type.value, "eq") && !StringUtil::CIEquals(type.value, "not-eq") &&
	    !StringUtil::CIEquals(type.value, "starts-with") && !StringUtil::CIEquals(type.value, "not-starts-with")) {
		return StringUtil::Format("LiteralExpression property 'type' must be one of [lt, lt-eq, gt, gt-eq, eq, not-eq, "
		                          "starts-with, not-starts-with], not %s",
		                          type.value);
	}
	error = term.Validate();
	if (!error.empty()) {
		return error;
	}
	error = value.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

yyjson_mut_val *LiteralExpression::ToJSON(yyjson_mut_doc *doc) const {
	throw InternalException("Can't serialize this class (LiteralExpression)");
}

} // namespace rest_api_objects
} // namespace duckdb
