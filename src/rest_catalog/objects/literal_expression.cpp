
#include "rest_catalog/objects/literal_expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

LiteralExpression::LiteralExpression()
    : term(GeneratedObjectAccess::Create<Term>()), value(GeneratedObjectAccess::Create<PrimitiveTypeValue>()) {
}

LiteralExpressionBuilder::LiteralExpressionBuilder() {
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetType(ExpressionType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetTerm(Term value) {
	result_.term = std::move(value);
	has_term_ = true;
	return *this;
}

LiteralExpressionBuilder &LiteralExpressionBuilder::SetValue(PrimitiveTypeValue value) {
	result_.value = std::move(value);
	has_value_ = true;
	return *this;
}

string LiteralExpressionBuilder::TryBuild(LiteralExpression &result) {
	if (!has_type_) {
		return "LiteralExpression required property 'type' is missing";
	}
	if (!has_term_) {
		return "LiteralExpression required property 'term' is missing";
	}
	if (!has_value_) {
		return "LiteralExpression required property 'value' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

LiteralExpression LiteralExpressionBuilder::Build() {
	LiteralExpression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

LiteralExpression LiteralExpression::FromJSON(yyjson_val *obj) {
	LiteralExpression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

LiteralExpression LiteralExpression::Copy() const {
	LiteralExpression res;
	res.type = type.Copy();
	res.term = term.Copy();
	res.value = value.Copy();
	return res;
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

string LiteralExpression::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "LiteralExpression required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto term_val = yyjson_obj_get(obj, "term");
	if (!term_val) {
		return "LiteralExpression required property 'term' is missing";
	} else {
		error = term.TryFromJSON(term_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto value_val = yyjson_obj_get(obj, "value");
	if (!value_val) {
		return "LiteralExpression required property 'value' is missing";
	} else {
		error = value.TryFromJSON(value_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

yyjson_mut_val *LiteralExpression::ToJSON(yyjson_mut_doc *doc) const {
	throw InternalException("Can't serialize this class (LiteralExpression)");
}

} // namespace rest_api_objects
} // namespace duckdb
