
#include "rest_catalog/objects/and_or_expression.hpp"

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

AndOrExpression::AndOrExpression(ExpressionType type_p, unique_ptr<Expression> left_p, unique_ptr<Expression> right_p)
    : type(std::move(type_p)), left(std::move(left_p)), right(std::move(right_p)) {
}

AndOrExpressionBuilder::AndOrExpressionBuilder() {
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetLeft(unique_ptr<Expression> value) {
	left_ = std::move(value);
	has_left_ = true;
	return *this;
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetRight(unique_ptr<Expression> value) {
	right_ = std::move(value);
	has_right_ = true;
	return *this;
}

AndOrExpression AndOrExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AndOrExpression required property 'type' is missing");
	}
	if (!has_left_) {
		throw InvalidInputException("AndOrExpression required property 'left' is missing");
	}
	if (!has_right_) {
		throw InvalidInputException("AndOrExpression required property 'right' is missing");
	}
	auto result = AndOrExpression(std::move(*type_), std::move(left_), std::move(right_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AndOrExpressionBuilder::TryBuild(optional<AndOrExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AndOrExpression::TryFromJSON(yyjson_val *obj, AndOrExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AndOrExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		auto left_val = yyjson_obj_get(obj, "left");
		if (!left_val) {
			throw InvalidInputException("AndOrExpression required property 'left' is missing");
		} else {
			unique_ptr<Expression> left;
			left = make_uniq<Expression>(Expression::FromJSON(left_val));
			builder.SetLeft(std::move(left));
		}
		auto right_val = yyjson_obj_get(obj, "right");
		if (!right_val) {
			throw InvalidInputException("AndOrExpression required property 'right' is missing");
		} else {
			unique_ptr<Expression> right;
			right = make_uniq<Expression>(Expression::FromJSON(right_val));
			builder.SetRight(std::move(right));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AndOrExpression AndOrExpression::FromJSON(yyjson_val *obj) {
	AndOrExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AndOrExpression AndOrExpression::Copy() const {
	AndOrExpressionBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	unique_ptr<Expression> left_tmp;
	left_tmp = left ? make_uniq<Expression>(left->Copy()) : nullptr;
	builder.SetLeft(std::move(left_tmp));
	unique_ptr<Expression> right_tmp;
	right_tmp = right ? make_uniq<Expression>(right->Copy()) : nullptr;
	builder.SetRight(std::move(right_tmp));
	return builder.Build();
}

string AndOrExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "and") && !StringUtil::CIEquals(type.value, "or")) {
		return StringUtil::Format("AndOrExpression property 'type' must be one of [and, or], not %s", type.value);
	}
	error = left->Validate();
	if (!error.empty()) {
		return error;
	}
	error = right->Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void AndOrExpression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: left
	yyjson_mut_val *left_val = left->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "left", left_val);

	// Serialize: right
	yyjson_mut_val *right_val = right->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "right", right_val);
}

yyjson_mut_val *AndOrExpression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
