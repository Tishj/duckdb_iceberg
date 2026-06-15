
#include "rest_catalog/objects/list_type.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ListType::ListType(string type_p, int32_t element_id_p, unique_ptr<Type> element_p, bool element_required_p)
    : type(std::move(type_p)), element_id(std::move(element_id_p)), element(std::move(element_p)),
      element_required(std::move(element_required_p)) {
}

ListTypeBuilder::ListTypeBuilder() {
}

ListTypeBuilder &ListTypeBuilder::SetType(string value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

ListTypeBuilder &ListTypeBuilder::SetElementId(int32_t value) {
	element_id_ = std::move(value);
	has_element_id_ = true;
	return *this;
}

ListTypeBuilder &ListTypeBuilder::SetElement(unique_ptr<Type> value) {
	element_ = std::move(value);
	has_element_ = true;
	return *this;
}

ListTypeBuilder &ListTypeBuilder::SetElementRequired(bool value) {
	element_required_ = std::move(value);
	has_element_required_ = true;
	return *this;
}

ListType ListTypeBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("ListType required property 'type' is missing");
	}
	if (!has_element_id_) {
		throw InvalidInputException("ListType required property 'element-id' is missing");
	}
	if (!has_element_) {
		throw InvalidInputException("ListType required property 'element' is missing");
	}
	if (!has_element_required_) {
		throw InvalidInputException("ListType required property 'element-required' is missing");
	}
	auto result =
	    ListType(std::move(*type_), std::move(*element_id_), std::move(element_), std::move(*element_required_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ListTypeBuilder::TryBuild(optional<ListType> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListType ListType::FromJSON(yyjson_val *obj) {
	ListTypeBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("ListType required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ListType property 'type' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto element_id_val = yyjson_obj_get(obj, "element-id");
	if (!element_id_val) {
		throw InvalidInputException("ListType required property 'element-id' is missing");
	} else {
		int32_t element_id;
		if (yyjson_is_int(element_id_val)) {
			element_id = yyjson_get_int(element_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ListType property 'element_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(element_id_val)));
		}
		builder.SetElementId(std::move(element_id));
	}
	auto element_val = yyjson_obj_get(obj, "element");
	if (!element_val) {
		throw InvalidInputException("ListType required property 'element' is missing");
	} else {
		unique_ptr<Type> element;
		element = make_uniq<Type>(Type::FromJSON(element_val));
		builder.SetElement(std::move(element));
	}
	auto element_required_val = yyjson_obj_get(obj, "element-required");
	if (!element_required_val) {
		throw InvalidInputException("ListType required property 'element-required' is missing");
	} else {
		bool element_required;
		if (yyjson_is_bool(element_required_val)) {
			element_required = yyjson_get_bool(element_required_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ListType property 'element_required' is not of type 'boolean', found '%s' instead",
			                       yyjson_get_type_desc(element_required_val)));
		}
		builder.SetElementRequired(std::move(element_required));
	}
	return builder.Build();
}

string ListType::TryFromJSON(yyjson_val *obj, optional<ListType> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListType ListType::Copy() const {
	ListTypeBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	int32_t element_id_tmp;
	element_id_tmp = element_id;
	builder.SetElementId(std::move(element_id_tmp));
	unique_ptr<Type> element_tmp;
	element_tmp = element ? make_uniq<Type>(element->Copy()) : nullptr;
	builder.SetElement(std::move(element_tmp));
	bool element_required_tmp;
	element_required_tmp = element_required;
	builder.SetElementRequired(std::move(element_required_tmp));
	return builder.Build();
}

string ListType::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "list")) {
		return StringUtil::Format("ListType property 'type' must be list, not %s", type);
	}
	error = element->Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void ListType::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: element-id
	yyjson_mut_obj_add_int(doc, obj, "element-id", element_id);

	// Serialize: element
	yyjson_mut_val *element_val = element->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "element", element_val);

	// Serialize: element-required
	yyjson_mut_obj_add_bool(doc, obj, "element-required", element_required);
}

yyjson_mut_val *ListType::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
