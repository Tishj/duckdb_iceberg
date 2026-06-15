
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

UnaryExpression::UnaryExpression() {
}

UnaryExpressionBuilder::UnaryExpressionBuilder() {
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetType(ExpressionType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

UnaryExpressionBuilder &UnaryExpressionBuilder::SetTerm(Term value) {
	result_.term = std::move(value);
	has_term_ = true;
	return *this;
}

string UnaryExpressionBuilder::TryBuild(UnaryExpression &result) {
	if (!has_type_) {
		return "UnaryExpression required property 'type' is missing";
	}
	if (!has_term_) {
		return "UnaryExpression required property 'term' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

UnaryExpression UnaryExpressionBuilder::Build() {
	UnaryExpression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

UnaryExpression UnaryExpression::FromJSON(yyjson_val *obj) {
	UnaryExpression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

UnaryExpression UnaryExpression::Copy() const {
	UnaryExpression res;
	res.type = type.Copy();
	res.term = term.Copy();
	return res;
}

string UnaryExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (type.value != "is-null" && type.value != "not-null" && type.value != "is-nan" && type.value != "not-nan") {
		return "UnaryExpression property 'type' must be one of [is-null, not-null, is-nan, not-nan]";
	}
	error = term.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string UnaryExpression::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "UnaryExpression required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto term_val = yyjson_obj_get(obj, "term");
	if (!term_val) {
		return "UnaryExpression required property 'term' is missing";
	} else {
		error = term.TryFromJSON(term_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

yyjson_mut_val *UnaryExpression::ToJSON(yyjson_mut_doc *doc) const {
	throw InternalException("Can't serialize this class (UnaryExpression)");
}

} // namespace rest_api_objects
} // namespace duckdb
