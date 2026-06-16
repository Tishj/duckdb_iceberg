
#include "rest_catalog/objects/true_expression.hpp"

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

TrueExpression::TrueExpression(ExpressionType type_p) : type(std::move(type_p)) {
}
TrueExpression::TrueExpression(const TrueExpression &other) : type(other.type.Copy()) {
}
TrueExpression::TrueExpression(TrueExpression &&other) : TrueExpression(static_cast<const TrueExpression &>(other)) {
}

TrueExpressionBuilder::TrueExpressionBuilder() {
}

TrueExpressionBuilder &TrueExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

TrueExpression TrueExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("TrueExpression required property 'type' is missing");
	}
	auto result = TrueExpression(std::move(*type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TrueExpressionBuilder::TryBuild(optional<TrueExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string TrueExpression::TryFromJSON(yyjson_val *obj, TrueExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("TrueExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TrueExpression TrueExpression::FromJSON(yyjson_val *obj) {
	TrueExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

TrueExpression TrueExpression::Copy() const {
	return TrueExpression(*this);
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
