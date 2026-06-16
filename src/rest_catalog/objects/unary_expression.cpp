
#include "rest_catalog/objects/unary_expression.hpp"

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

UnaryExpression::UnaryExpression(ExpressionType type_p, Term term_p)
    : type(std::move(type_p)), term(std::move(term_p)) {
}

UnaryExpressionBuilder::UnaryExpressionBuilder() {
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetTerm(Term value) {
	term_.emplace(std::move(value));
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

string UnaryExpression::TryFromJSON(yyjson_val *obj, UnaryExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("UnaryExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		auto term_val = yyjson_obj_get(obj, "term");
		if (!term_val) {
			throw InvalidInputException("UnaryExpression required property 'term' is missing");
		} else {
			builder.SetTerm(Term::FromJSON(term_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UnaryExpression UnaryExpression::FromJSON(yyjson_val *obj) {
	UnaryExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

UnaryExpression UnaryExpression::Copy() const {
	UnaryExpressionBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	auto term_tmp = term.Copy();
	builder.SetTerm(std::move(term_tmp));
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
