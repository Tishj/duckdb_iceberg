
#include "rest_catalog/objects/not_expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

NotExpression::NotExpression() : child(GeneratedObjectAccess::Create<unique_ptr<Expression>>()) {
}

NotExpressionBuilder::NotExpressionBuilder() {
}

NotExpressionBuilder &NotExpressionBuilder::SetType(ExpressionType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

NotExpressionBuilder &NotExpressionBuilder::SetChild(unique_ptr<Expression> value) {
	result_.child = std::move(value);
	has_child_ = true;
	return *this;
}

string NotExpressionBuilder::TryBuild(NotExpression &result) {
	if (!has_type_) {
		return "NotExpression required property 'type' is missing";
	}
	if (!has_child_) {
		return "NotExpression required property 'child' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

NotExpression NotExpressionBuilder::Build() {
	NotExpression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

NotExpression NotExpression::FromJSON(yyjson_val *obj) {
	NotExpression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

NotExpression NotExpression::Copy() const {
	NotExpression res;
	res.type = type.Copy();
	res.child = child ? make_uniq<Expression>(child->Copy()) : nullptr;
	return res;
}

string NotExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "not")) {
		return StringUtil::Format("NotExpression property 'type' must be not, not %s", type.value);
	}
	error = child->Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string NotExpression::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "NotExpression required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto child_val = yyjson_obj_get(obj, "child");
	if (!child_val) {
		return "NotExpression required property 'child' is missing";
	} else {
		child = GeneratedObjectAccess::CreateUnique<Expression>();
		error = child->TryFromJSON(child_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void NotExpression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: child
	yyjson_mut_val *child_val = child->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "child", child_val);
}

yyjson_mut_val *NotExpression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
