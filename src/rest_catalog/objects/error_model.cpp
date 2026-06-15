
#include "rest_catalog/objects/error_model.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ErrorModel::ErrorModel(string message_p, string type_p, int32_t code_p, optional<vector<string>> stack_p)
    : message(std::move(message_p)), type(std::move(type_p)), code(std::move(code_p)), stack(std::move(stack_p)) {
}

ErrorModelBuilder::ErrorModelBuilder() {
}

ErrorModelBuilder &ErrorModelBuilder::SetMessage(string value) {
	message_ = std::move(value);
	has_message_ = true;
	return *this;
}

ErrorModelBuilder &ErrorModelBuilder::SetType(string value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

ErrorModelBuilder &ErrorModelBuilder::SetCode(int32_t value) {
	code_ = std::move(value);
	has_code_ = true;
	return *this;
}

ErrorModelBuilder &ErrorModelBuilder::SetStack(vector<string> value) {
	stack_ = std::move(value);
	return *this;
}

ErrorModel ErrorModelBuilder::Build() {
	if (!has_message_) {
		throw InvalidInputException("ErrorModel required property 'message' is missing");
	}
	if (!has_type_) {
		throw InvalidInputException("ErrorModel required property 'type' is missing");
	}
	if (!has_code_) {
		throw InvalidInputException("ErrorModel required property 'code' is missing");
	}
	auto result = ErrorModel(std::move(*message_), std::move(*type_), std::move(*code_), std::move(stack_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ErrorModelBuilder::TryBuild(optional<ErrorModel> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ErrorModel ErrorModel::FromJSON(yyjson_val *obj) {
	ErrorModelBuilder builder;
	auto message_val = yyjson_obj_get(obj, "message");
	if (!message_val) {
		throw InvalidInputException("ErrorModel required property 'message' is missing");
	} else {
		string message;
		if (yyjson_is_str(message_val)) {
			message = yyjson_get_str(message_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ErrorModel property 'message' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(message_val)));
		}
		builder.SetMessage(std::move(message));
	}
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("ErrorModel required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ErrorModel property 'type' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto code_val = yyjson_obj_get(obj, "code");
	if (!code_val) {
		throw InvalidInputException("ErrorModel required property 'code' is missing");
	} else {
		int32_t code;
		if (yyjson_is_int(code_val)) {
			code = yyjson_get_int(code_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ErrorModel property 'code' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(code_val)));
		}
		builder.SetCode(std::move(code));
	}
	auto stack_val = yyjson_obj_get(obj, "stack");
	if (stack_val) {
		vector<string> stack;
		if (yyjson_is_arr(stack_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(stack_val, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ErrorModel property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				stack.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("ErrorModel property 'stack' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(stack_val));
		}
		builder.SetStack(std::move(stack));
	}
	return builder.Build();
}

string ErrorModel::TryFromJSON(yyjson_val *obj, optional<ErrorModel> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ErrorModel ErrorModel::Copy() const {
	ErrorModelBuilder builder;
	string message_tmp;
	message_tmp = message;
	builder.SetMessage(std::move(message_tmp));
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	int32_t code_tmp;
	code_tmp = code;
	builder.SetCode(std::move(code_tmp));
	vector<string> stack_tmp;
	if (stack.has_value()) {
		stack_tmp.emplace();
		(*stack_tmp).reserve((*stack).size());
		for (auto &item : (*stack)) {
			(*stack_tmp).emplace_back(item);
		}
	}
	if (stack_tmp.has_value()) {
		builder.SetStack(std::move(stack_tmp));
	}
	return builder.Build();
}

string ErrorModel::Validate() const {
	string error;
	if (code < 400) {
		return "ErrorModel property 'code' must be at least 400";
	}
	if (code > 600) {
		return "ErrorModel property 'code' must be at most 600";
	}
	return "";
}

void ErrorModel::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: message
	yyjson_mut_obj_add_strcpy(doc, obj, "message", message.c_str());

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: code
	yyjson_mut_obj_add_int(doc, obj, "code", code);

	// Serialize: stack
	if (stack.has_value()) {
		auto &stack_value = *stack;
		yyjson_mut_val *stack_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : stack_value) {
			yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
			yyjson_mut_arr_append(stack_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "stack", stack_value_arr);
	}
}

yyjson_mut_val *ErrorModel::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
