
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
UnaryExpression::UnaryExpression(const UnaryExpression &other) : type(other.type.Copy()), term(other.term.Copy()) {
}
UnaryExpression::UnaryExpression(UnaryExpression &&other)
    : UnaryExpression(static_cast<const UnaryExpression &>(other)) {
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> UnaryExpressionBuilder::TryBuild(optional<UnaryExpression> &result) {
	if (!has_type_) {
		return "UnaryExpression required property 'type' is missing";
	}
	if (!has_term_) {
		return "UnaryExpression required property 'term' is missing";
	}
	auto built = UnaryExpression(std::move(*type_), std::move(*term_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> UnaryExpression::TryFromJSON(yyjson_val *obj, UnaryExpressionBuilder &builder) {
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UnaryExpression UnaryExpression::FromJSON(yyjson_val *obj) {
	UnaryExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

UnaryExpression UnaryExpression::Copy() const {
	return UnaryExpression(*this);
}

optional<string> UnaryExpression::Validate() const {
	optional<string> error;
	error = type.Validate();
	if (error) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "is-null") && !StringUtil::CIEquals(type.value, "not-null") &&
	    !StringUtil::CIEquals(type.value, "is-nan") && !StringUtil::CIEquals(type.value, "not-nan")) {
		return StringUtil::Format(
		    "UnaryExpression property 'type' must be one of [is-null, not-null, is-nan, not-nan], not %s", type.value);
	}
	error = term.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

yyjson_mut_val *UnaryExpression::ToJSON(yyjson_mut_doc *doc) const {
	throw InternalException("Can't serialize this class (UnaryExpression)");
}

} // namespace rest_api_objects
} // namespace duckdb
