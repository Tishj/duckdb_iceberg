
#include "rest_catalog/objects/and_or_expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AndOrExpression::AndOrExpression() {
}

AndOrExpressionBuilder::AndOrExpressionBuilder() {
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetType(ExpressionType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetLeft(unique_ptr<Expression> value) {
	result_.left = std::move(value);
	has_left_ = true;
	return *this;
}

AndOrExpressionBuilder &AndOrExpressionBuilder::SetRight(unique_ptr<Expression> value) {
	result_.right = std::move(value);
	has_right_ = true;
	return *this;
}

string AndOrExpressionBuilder::TryBuild(AndOrExpression &result) {
	if (!has_type_) {
		return "AndOrExpression required property 'type' is missing";
	}
	if (!has_left_) {
		return "AndOrExpression required property 'left' is missing";
	}
	if (!has_right_) {
		return "AndOrExpression required property 'right' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AndOrExpression AndOrExpressionBuilder::Build() {
	AndOrExpression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AndOrExpression AndOrExpression::FromJSON(yyjson_val *obj) {
	AndOrExpression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AndOrExpression AndOrExpression::Copy() const {
	AndOrExpression res;
	res.type = type.Copy();
	res.left = left ? make_uniq<Expression>(left->Copy()) : nullptr;
	res.right = right ? make_uniq<Expression>(right->Copy()) : nullptr;
	return res;
}

string AndOrExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (type.value != "and" && type.value != "or") {
		return "AndOrExpression property 'type' must be one of [and, or]";
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

string AndOrExpression::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AndOrExpression required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto left_val = yyjson_obj_get(obj, "left");
	if (!left_val) {
		return "AndOrExpression required property 'left' is missing";
	} else {
		left = make_uniq<Expression>();
		error = left->TryFromJSON(left_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto right_val = yyjson_obj_get(obj, "right");
	if (!right_val) {
		return "AndOrExpression required property 'right' is missing";
	} else {
		right = make_uniq<Expression>();
		error = right->TryFromJSON(right_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
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
