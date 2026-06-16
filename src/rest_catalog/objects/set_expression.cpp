
#include "rest_catalog/objects/set_expression.hpp"

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

SetExpression::SetExpression(ExpressionType type_p, Term term_p, vector<PrimitiveTypeValue> values_p)
    : type(std::move(type_p)), term(std::move(term_p)), values(std::move(values_p)) {
}
SetExpression::SetExpression(const SetExpression &other)
    : type(other.type.Copy()), term(other.term.Copy()), values(([&]() {
	      vector<PrimitiveTypeValue> copied;
	      copied.reserve(other.values.size());
	      for (const auto &item : other.values) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }())) {
}
SetExpression::SetExpression(SetExpression &&other) : SetExpression(static_cast<const SetExpression &>(other)) {
}

SetExpressionBuilder::SetExpressionBuilder() {
}

SetExpressionBuilder &SetExpressionBuilder::SetType(ExpressionType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

SetExpressionBuilder &SetExpressionBuilder::SetTerm(Term value) {
	term_.emplace(std::move(value));
	has_term_ = true;
	return *this;
}

SetExpressionBuilder &SetExpressionBuilder::SetValues(vector<PrimitiveTypeValue> value) {
	values_.emplace(std::move(value));
	has_values_ = true;
	return *this;
}

SetExpression SetExpressionBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("SetExpression required property 'type' is missing");
	}
	if (!has_term_) {
		throw InvalidInputException("SetExpression required property 'term' is missing");
	}
	if (!has_values_) {
		throw InvalidInputException("SetExpression required property 'values' is missing");
	}
	auto result = SetExpression(std::move(*type_), std::move(*term_), std::move(*values_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetExpressionBuilder::TryBuild(optional<SetExpression> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string SetExpression::TryFromJSON(yyjson_val *obj, SetExpressionBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("SetExpression required property 'type' is missing");
		} else {
			builder.SetType(ExpressionType::FromJSON(type_val));
		}
		auto term_val = yyjson_obj_get(obj, "term");
		if (!term_val) {
			throw InvalidInputException("SetExpression required property 'term' is missing");
		} else {
			builder.SetTerm(Term::FromJSON(term_val));
		}
		auto values_val = yyjson_obj_get(obj, "values");
		if (!values_val) {
			throw InvalidInputException("SetExpression required property 'values' is missing");
		} else {
			vector<PrimitiveTypeValue> values;
			if (yyjson_is_arr(values_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(values_val, idx, max, val) {
					auto tmp = PrimitiveTypeValue::FromJSON(val);
					values.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("SetExpression property 'values' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(values_val)));
			}
			builder.SetValues(std::move(values));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetExpression SetExpression::FromJSON(yyjson_val *obj) {
	SetExpressionBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

SetExpression SetExpression::Copy() const {
	return SetExpression(*this);
}

string SetExpression::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "in") && !StringUtil::CIEquals(type.value, "not-in")) {
		return StringUtil::Format("SetExpression property 'type' must be one of [in, not-in], not %s", type.value);
	}
	error = term.Validate();
	if (!error.empty()) {
		return error;
	}
	for (const auto &item : values) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void SetExpression::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: term
	yyjson_mut_val *term_val = term.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "term", term_val);

	// Serialize: values
	yyjson_mut_val *values_arr = yyjson_mut_arr(doc);
	for (const auto &item : values) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(values_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "values", values_arr);
}

yyjson_mut_val *SetExpression::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
