
#include "rest_catalog/objects/not_expression.hpp"

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

NotExpression::NotExpression(ExpressionType type_p, unique_ptr<Expression> child_p)
    : type(std::move(type_p)), child(std::move(child_p)) {
}
NotExpression::NotExpression(const NotExpression &other)
    : type(other.type.Copy()), child(other.child ? make_uniq<Expression>(other.child->Copy()) : nullptr) {
}
NotExpression::NotExpression(NotExpression &&other) : NotExpression(static_cast<const NotExpression &>(other)) {
}

NotExpressionBuilder::NotExpressionBuilder() {
}

NotExpressionBuilder &NotExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

NotExpressionBuilder &NotExpressionBuilder::SetChild(unique_ptr<Expression> value) {
	child_ = std::move(value);
	has_child_ = true;
	return *this;
}

NotExpression NotExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("NotExpression required property 'type' is missing");
	}
	if (!has_child_) {
		throw InvalidInputException("NotExpression required property 'child' is missing");
	}
	auto result = NotExpression(std::move(*type_), std::move(child_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> NotExpressionBuilder::TryBuild(optional<NotExpression> &result) {
	if (!has_type_) {
		return "NotExpression required property 'type' is missing";
	}
	if (!has_child_) {
		return "NotExpression required property 'child' is missing";
	}
	auto built = NotExpression(std::move(*type_), std::move(child_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> NotExpression::TryFromJSON(yyjson_val *obj, NotExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("NotExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		auto child_val = yyjson_obj_get(obj, "child");
		if (!child_val) {
			throw InvalidInputException("NotExpression required property 'child' is missing");
		} else {
			unique_ptr<Expression> child;
			child = make_uniq<Expression>(Expression::FromJSON(child_val));
			builder.SetChild(std::move(child));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

NotExpression NotExpression::FromJSON(yyjson_val *obj) {
	NotExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

NotExpression NotExpression::Copy() const {
	return NotExpression(*this);
}

optional<string> NotExpression::Validate() const {
	optional<string> error;
	error = type.Validate();
	if (error) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "not")) {
		return StringUtil::Format("NotExpression property 'type' must be not, not %s", type.value);
	}
	error = child->Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
