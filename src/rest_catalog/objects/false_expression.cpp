
#include "rest_catalog/objects/false_expression.hpp"

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

FalseExpression::FalseExpression(ExpressionType type_p) : type(std::move(type_p)) {
}

FalseExpressionBuilder::FalseExpressionBuilder() {
}

FalseExpressionBuilder &FalseExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

FalseExpression FalseExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("FalseExpression required property 'type' is missing");
	}
	auto result = FalseExpression(std::move(*type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FalseExpressionBuilder::TryBuild(optional<FalseExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string FalseExpression::TryFromJSON(yyjson_val *obj, FalseExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("FalseExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FalseExpression FalseExpression::FromJSON(yyjson_val *obj) {
	FalseExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

FalseExpression FalseExpression::Copy() const {
	FalseExpressionBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	return builder.Build();
}

string FalseExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "false")) {
		return StringUtil::Format("FalseExpression property 'type' must be false, not %s", type.value);
	}
	return "";
}

void FalseExpression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);
}

yyjson_mut_val *FalseExpression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
