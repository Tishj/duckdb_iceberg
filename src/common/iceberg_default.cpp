#include "common/iceberg_default.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/function/scalar/struct_functions.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/planner/expression/bound_columnref_expression.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "catalog/rest/api/iceberg_type.hpp"

namespace duckdb {

IcebergDefaultBinder::IcebergDefaultBinder(ClientContext &context)
    : context(context), binder(Binder::CreateBinder(context)), constant_binder(*binder, context, "DEFAULT") {
}

namespace {

static void ValidateDefaultValue(const Value &value, const LogicalType &type) {
	if (value.IsNull()) {
		return;
	}
	switch (type.id()) {
	case LogicalTypeId::SQLNULL:
	case LogicalTypeId::VARIANT:
	// case LogicalTypeId::GEOGRAPHY:
	case LogicalTypeId::GEOMETRY:
		throw InvalidInputException("Non-null DEFAULT values are not accepted for columns of type %s",
		                            IcebergTypeHelper::LogicalTypeToIcebergType(type));
	default:
		return;
	}
}

static optional_idx FindStructField(const LogicalType &type, const Identifier &name) {
	auto &children = StructType::GetChildTypes(type);
	for (idx_t child_idx = 0; child_idx < children.size(); child_idx++) {
		if (children[child_idx].first == name) {
			return child_idx;
		}
	}
	return optional_idx();
}

static Value ParseStructDefault(const Value &value, const LogicalType &type) {
	if (value.IsNull()) {
		return Value(type);
	}
	if (value.type().id() != LogicalTypeId::STRUCT) {
		throw InvalidInputException("Non-null DEFAULT values for STRUCT must be a struct containing the field '%s'",
		                            ICEBERG_STRUCT_DEFAULT_FIELD);
	}

	auto &target_children = StructType::GetChildTypes(type);
	for (auto &target_child : target_children) {
		if (target_child.first == ICEBERG_STRUCT_DEFAULT_FIELD) {
			throw InvalidInputException("The field name '%s' is reserved in STRUCT types that have a DEFAULT value",
			                            ICEBERG_STRUCT_DEFAULT_FIELD);
		}
	}

	vector<Value> result_children;
	result_children.reserve(target_children.size());
	for (auto &target_child : target_children) {
		result_children.emplace_back(target_child.second);
	}

	bool found_struct_default = false;
	auto &input_children = StructType::GetChildTypes(value.type());
	auto &input_values = StructValue::GetChildren(value);
	for (idx_t child_idx = 0; child_idx < input_children.size(); child_idx++) {
		auto &input_name = input_children[child_idx].first;
		auto &input_value = input_values[child_idx];
		if (input_name == ICEBERG_STRUCT_DEFAULT_FIELD) {
			found_struct_default = true;
			if (!input_value.IsNull()) {
				throw InvalidInputException("The field '%s' only supports NULL as its value",
				                            ICEBERG_STRUCT_DEFAULT_FIELD);
			}
			continue;
		}

		auto target_idx = FindStructField(type, input_name);
		if (!target_idx.IsValid()) {
			throw InvalidInputException("DEFAULT field '%s' does not exist in STRUCT type %s",
			                            input_name.GetIdentifierName(), type.ToString());
		}
		auto &target_type = target_children[target_idx.GetIndex()].second;
		if (target_type.id() == LogicalTypeId::STRUCT) {
			result_children[target_idx.GetIndex()] = ParseStructDefault(input_value, target_type);
		} else {
			ValidateDefaultValue(input_value, target_type);
			result_children[target_idx.GetIndex()] = input_value.DefaultCastAs(target_type);
		}
	}
	if (!found_struct_default) {
		throw InvalidInputException("Non-null DEFAULT values for STRUCT must contain the field '%s'",
		                            ICEBERG_STRUCT_DEFAULT_FIELD);
	}
	return Value::STRUCT(type, std::move(result_children));
}

} // namespace

Value IcebergDefaultBinder::Evaluate(optional_ptr<const ParsedExpression> expr, const LogicalType &type) {
	if (!expr) {
		return Value(type);
	}
	auto expr_copy = expr->Copy();
	auto bound_expr = constant_binder.Bind(expr_copy, nullptr);
	if (!bound_expr->IsFoldable()) {
		throw NotImplementedException("Only foldable expressions are allowed as DEFAULT values");
	}
	auto default_value = ExpressionExecutor::EvaluateScalar(context, *bound_expr, false);
	if (type.id() == LogicalTypeId::STRUCT) {
		return ParseStructDefault(default_value, type);
	}
	ValidateDefaultValue(default_value, type);
	return default_value.DefaultCastAs(type);
}

namespace {

//! Used to determine if the field of a struct is mapped or not
struct StructFieldMapping {
	case_insensitive_map_t<unique_ptr<StructFieldMapping>> child_mapping;
};

static Value CreateStructMapping(const LogicalType &struct_type, const string &name,
                                 case_insensitive_map_t<unique_ptr<StructFieldMapping>> &out_mapping) {
	child_list_t<Value> field_mapping;

	auto &struct_children = StructType::GetChildTypes(struct_type);
	for (auto &[field_name, field_type] : struct_children) {
		auto &child_mapping = out_mapping[field_name.GetIdentifierName()];
		if (!child_mapping) {
			child_mapping = make_uniq<StructFieldMapping>();
		}
		Value mapping;
		if (field_type.id() == LogicalTypeId::STRUCT) {
			mapping = CreateStructMapping(field_type, field_name.GetIdentifierName(), child_mapping->child_mapping);
		} else {
			mapping = Value(field_name);
		}
		field_mapping.emplace_back(field_name, mapping);
	}
	auto struct_value = Value::STRUCT(field_mapping);
	if (name.empty()) {
		//! Root column
		return struct_value;
	}
	return Value::TUPLE({Value(name), struct_value});
}

static Value CreateStructDefault(const Value &value,
                                 const case_insensitive_map_t<unique_ptr<StructFieldMapping>> &mapping = {}) {
	child_list_t<Value> field_defaults;
	auto &field_values = StructValue::GetChildren(value);
	auto &struct_children = StructType::GetChildTypes(value.type());
	for (idx_t j = 0; j < field_values.size(); j++) {
		auto &field_name = struct_children[j].first;
		auto &field_type = struct_children[j].second;
		auto &field_value = field_values[j];
		if (field_name == ICEBERG_STRUCT_DEFAULT_FIELD) {
			continue;
		}

		auto it = mapping.find(field_name.GetIdentifierName());
		const bool is_mapped = it != mapping.end();

		Value field_default;
		if (field_type.id() == LogicalTypeId::STRUCT) {
			if (is_mapped) {
				field_default = CreateStructDefault(field_value, it->second->child_mapping);
			} else {
				auto struct_default_idx = FindStructField(field_type, Identifier(ICEBERG_STRUCT_DEFAULT_FIELD));
				if (!struct_default_idx.IsValid()) {
					throw InternalException("Missing '%s' in internal STRUCT default", ICEBERG_STRUCT_DEFAULT_FIELD);
				}
				field_default = StructValue::GetChildren(field_value)[struct_default_idx.GetIndex()];
			}

			if (field_default.IsNull()) {
				if (is_mapped) {
					//! The input supplies this struct, so no whole-struct fallback is needed.
					continue;
				}
				field_defaults.emplace_back(field_name, std::move(field_default));
				continue;
			}
		} else {
			if (is_mapped) {
				continue;
			}
			field_default = field_value;
		}

		field_defaults.emplace_back(field_name, field_default);
	}
	if (field_defaults.empty()) {
		//! Skipped all fields, signal that the value should be omitted
		return Value();
	}
	return Value::STRUCT(field_defaults);
}

static Value EvaluateStructDefault(ClientContext &context, const Expression &default_expr) {
	if (!default_expr.IsFoldable()) {
		throw BinderException("Cannot resolve partial STRUCT insert with non-constant default value");
	}
	if (default_expr.GetExpressionClass() == ExpressionClass::BOUND_FUNCTION) {
		auto &function_expr = default_expr.Cast<BoundFunctionExpression>();
		if (function_expr.Function().GetName() == "constant_or_null" && function_expr.GetChildren().size() == 2) {
			return ExpressionExecutor::EvaluateScalar(context, *function_expr.GetChildren()[1]);
		}
	}
	Value default_value;
	if (!ExpressionExecutor::TryEvaluateScalar(context, default_expr, default_value)) {
		throw BinderException("Cannot resolve partial STRUCT insert with non-constant default value");
	}
	return default_value;
}

} // namespace

unique_ptr<Expression> IcebergDefaultProjectionResolver::ResolveDefault(ClientContext &context,
                                                                        const LogicalType &input_type,
                                                                        const LogicalType &result_type,
                                                                        ColumnBinding binding,
                                                                        const Expression &default_expr) {
	auto default_value = EvaluateStructDefault(context, default_expr);
	if (default_value.IsNull() || input_type.id() != LogicalTypeId::STRUCT ||
	    result_type.id() != LogicalTypeId::STRUCT) {
		return make_uniq<BoundColumnRefExpression>(input_type, binding);
	}

	// Column is of type STRUCT, create a remap that fills in omitted fields from the column default.
	vector<unique_ptr<Expression>> children;
	children.push_back(make_uniq<BoundColumnRefExpression>(input_type, binding));
	children.push_back(make_uniq<BoundConstantExpression>(Value(result_type)));

	case_insensitive_map_t<unique_ptr<StructFieldMapping>> mapping;
	children.push_back(make_uniq<BoundConstantExpression>(CreateStructMapping(input_type, "", mapping)));
	children.push_back(make_uniq<BoundConstantExpression>(CreateStructDefault(default_value, mapping)));
	return RemapStructFun::GetFunction().Bind(context, std::move(children));
}

} // namespace duckdb
