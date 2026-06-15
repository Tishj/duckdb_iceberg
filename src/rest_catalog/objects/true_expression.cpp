
#include "rest_catalog/objects/true_expression.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TrueExpression::TrueExpression() {
}

TrueExpressionBuilder::TrueExpressionBuilder() {
}

TrueExpressionBuilder &TrueExpressionBuilder::SetType(ExpressionType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

string TrueExpressionBuilder::TryBuild(TrueExpression &result) {
	if (!has_type_) {
		return "TrueExpression required property 'type' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

TrueExpression TrueExpressionBuilder::Build() {
	TrueExpression result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

TrueExpression TrueExpression::FromJSON(yyjson_val *obj) {
	TrueExpression res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

TrueExpression TrueExpression::Copy() const {
	TrueExpression res;
	res.type = type.Copy();
	return res;
}

string TrueExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "true")) {
		return StringUtil::Format("TrueExpression property 'type' must be true, not %s", type.value);
	}
	return "";
}

string TrueExpression::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "TrueExpression required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
}

void TrueExpression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);
}

yyjson_mut_val *TrueExpression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
