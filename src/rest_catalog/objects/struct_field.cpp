
#include "rest_catalog/objects/struct_field.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

StructField::StructField()
    : type(GeneratedObjectAccess::Create<unique_ptr<Type>>()),
      initial_default(GeneratedObjectAccess::Create<optional<PrimitiveTypeValue>>()),
      write_default(GeneratedObjectAccess::Create<optional<PrimitiveTypeValue>>()) {
}

StructFieldBuilder::StructFieldBuilder() {
}

StructFieldBuilder &StructFieldBuilder::SetId(int32_t value) {
	result_.id = std::move(value);
	has_id_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetName(string value) {
	result_.name = std::move(value);
	has_name_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetType(unique_ptr<Type> value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetRequired(bool value) {
	result_.required = std::move(value);
	has_required_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetDoc(string value) {
	result_._doc = std::move(value);
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetInitialDefault(PrimitiveTypeValue value) {
	result_.initial_default = std::move(value);
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetWriteDefault(PrimitiveTypeValue value) {
	result_.write_default = std::move(value);
	return *this;
}

string StructFieldBuilder::TryBuild(StructField &result) {
	if (!has_id_) {
		return "StructField required property 'id' is missing";
	}
	if (!has_name_) {
		return "StructField required property 'name' is missing";
	}
	if (!has_type_) {
		return "StructField required property 'type' is missing";
	}
	if (!has_required_) {
		return "StructField required property 'required' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

StructField StructFieldBuilder::Build() {
	StructField result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

StructField StructField::FromJSON(yyjson_val *obj) {
	StructField res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

StructField StructField::Copy() const {
	StructField res;
	res.id = id;
	res.name = name;
	res.type = type ? make_uniq<Type>(type->Copy()) : nullptr;
	res.required = required;
	if (_doc.has_value()) {
		res._doc.emplace();
		(*res._doc) = (*_doc);
	}
	if (initial_default.has_value()) {
		res.initial_default = GeneratedObjectAccess::Create<PrimitiveTypeValue>();
		(*res.initial_default) = (*initial_default).Copy();
	}
	if (write_default.has_value()) {
		res.write_default = GeneratedObjectAccess::Create<PrimitiveTypeValue>();
		(*res.write_default) = (*write_default).Copy();
	}
	return res;
}

string StructField::Validate() const {
	string error;
	error = type->Validate();
	if (!error.empty()) {
		return error;
	}
	if (initial_default.has_value()) {
		error = (*initial_default).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (write_default.has_value()) {
		error = (*write_default).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

string StructField::TryFromJSON(yyjson_val *obj) {
	string error;
	auto id_val = yyjson_obj_get(obj, "id");
	if (!id_val) {
		return "StructField required property 'id' is missing";
	} else {
		if (yyjson_is_int(id_val)) {
			id = yyjson_get_int(id_val);
		} else {
			return StringUtil::Format("StructField property 'id' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(id_val));
		}
	}
	auto name_val = yyjson_obj_get(obj, "name");
	if (!name_val) {
		return "StructField required property 'name' is missing";
	} else {
		if (yyjson_is_str(name_val)) {
			name = yyjson_get_str(name_val);
		} else {
			return StringUtil::Format("StructField property 'name' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(name_val));
		}
	}
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "StructField required property 'type' is missing";
	} else {
		type = GeneratedObjectAccess::CreateUnique<Type>();
		error = type->TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto required_val = yyjson_obj_get(obj, "required");
	if (!required_val) {
		return "StructField required property 'required' is missing";
	} else {
		if (yyjson_is_bool(required_val)) {
			required = yyjson_get_bool(required_val);
		} else {
			return StringUtil::Format("StructField property 'required' is not of type 'boolean', found '%s' instead",
			                          yyjson_get_type_desc(required_val));
		}
	}
	auto _doc_val = yyjson_obj_get(obj, "doc");
	if (_doc_val) {
		string _doc_tmp;
		if (yyjson_is_str(_doc_val)) {
			_doc_tmp = yyjson_get_str(_doc_val);
		} else {
			return StringUtil::Format("StructField property '_doc_tmp' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(_doc_val));
		}
		_doc = std::move(_doc_tmp);
	}
	auto initial_default_val = yyjson_obj_get(obj, "initial-default");
	if (initial_default_val) {
		auto initial_default_tmp = GeneratedObjectAccess::Create<PrimitiveTypeValue>();
		error = initial_default_tmp.TryFromJSON(initial_default_val);
		if (!error.empty()) {
			return error;
		}
		initial_default = std::move(initial_default_tmp);
	}
	auto write_default_val = yyjson_obj_get(obj, "write-default");
	if (write_default_val) {
		auto write_default_tmp = GeneratedObjectAccess::Create<PrimitiveTypeValue>();
		error = write_default_tmp.TryFromJSON(write_default_val);
		if (!error.empty()) {
			return error;
		}
		write_default = std::move(write_default_tmp);
	}
	return Validate();
}

void StructField::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: id
	yyjson_mut_obj_add_int(doc, obj, "id", id);

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: type
	yyjson_mut_val *type_val = type->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: required
	yyjson_mut_obj_add_bool(doc, obj, "required", required);

	// Serialize: doc
	if (_doc.has_value()) {
		auto &_doc_value = *_doc;
		yyjson_mut_obj_add_strcpy(doc, obj, "doc", _doc_value.c_str());
	}

	// Serialize: initial-default
	if (initial_default.has_value()) {
		auto &initial_default_value = *initial_default;
		yyjson_mut_val *initial_default_value_val = initial_default_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "initial-default", initial_default_value_val);
	}

	// Serialize: write-default
	if (write_default.has_value()) {
		auto &write_default_value = *write_default;
		yyjson_mut_val *write_default_value_val = write_default_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "write-default", write_default_value_val);
	}
}

yyjson_mut_val *StructField::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
