
#include "rest_catalog/objects/unary_expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

UnaryExpression::UnaryExpression(ExpressionType type_p, Term term_p)
    : type(std::move(type_p)), term(std::move(term_p)) {
}

UnaryExpressionBuilder::UnaryExpressionBuilder() {
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetType(ExpressionType value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetTerm(Term value) {
	term_ = std::move(value);
	has_term_ = true;
	return *this;
}

UnaryExpression UnaryExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("UnaryExpression required property 'type' is missing");
	}
	if (!has_term_) {
		throw InvalidInputException("UnaryExpression required property 'term' is missing");
	}
	auto result = UnaryExpression(std::move(*type_), std::move(*term_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string UnaryExpressionBuilder::TryBuild(optional<UnaryExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UnaryExpression UnaryExpression::FromJSON(yyjson_val *obj) {
	UnaryExpressionBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("UnaryExpression required property 'type' is missing");
	} else {
		optional<ExpressionType> type;
		type = ExpressionType::FromJSON(type_val);
		builder.SetType(std::move(*type));
	}
	auto term_val = yyjson_obj_get(obj, "term");
	if (!term_val) {
		throw InvalidInputException("UnaryExpression required property 'term' is missing");
	} else {
		optional<Term> term;
		term = Term::FromJSON(term_val);
		builder.SetTerm(std::move(*term));
	}
	return builder.Build();
}

string UnaryExpression::TryFromJSON(yyjson_val *obj, optional<UnaryExpression> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UnaryExpression UnaryExpression::Copy() const {
	UnaryExpressionBuilder builder;
	optional<ExpressionType> type_tmp;
	type_tmp = type.Copy();
	builder.SetType(std::move(*type_tmp));
	optional<Term> term_tmp;
	term_tmp = term.Copy();
	builder.SetTerm(std::move(*term_tmp));
	return builder.Build();
}

string UnaryExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "is-null") && !StringUtil::CIEquals(type.value, "not-null") &&
	    !StringUtil::CIEquals(type.value, "is-nan") && !StringUtil::CIEquals(type.value, "not-nan")) {
		return StringUtil::Format(
		    "UnaryExpression property 'type' must be one of [is-null, not-null, is-nan, not-nan], not %s", type.value);
	}
	error = term.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

yyjson_mut_val *UnaryExpression::ToJSON(yyjson_mut_doc *doc) const {
	throw InternalException("Can't serialize this class (UnaryExpression)");
}

} // namespace rest_api_objects
} // namespace duckdb
