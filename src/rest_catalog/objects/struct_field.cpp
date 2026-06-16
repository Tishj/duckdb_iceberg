
#include "rest_catalog/objects/struct_field.hpp"

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

StructField::StructField(int32_t id_p, string name_p, unique_ptr<Type> type_p, bool required_p, optional<string> _doc_p,
                         optional<PrimitiveTypeValue> initial_default_p, optional<PrimitiveTypeValue> write_default_p)
    : id(std::move(id_p)), name(std::move(name_p)), type(std::move(type_p)), required(std::move(required_p)),
      _doc(std::move(_doc_p)), initial_default(std::move(initial_default_p)),
      write_default(std::move(write_default_p)) {
}

StructFieldBuilder::StructFieldBuilder() {
}

StructFieldBuilder &StructFieldBuilder::SetId(int32_t value) {
	id_.emplace(std::move(value));
	has_id_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetType(unique_ptr<Type> value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetRequired(bool value) {
	required_.emplace(std::move(value));
	has_required_ = true;
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetDoc(string value) {
	_doc_.emplace(std::move(value));
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetInitialDefault(PrimitiveTypeValue value) {
	initial_default_.emplace(std::move(value));
	return *this;
}

StructFieldBuilder &StructFieldBuilder::SetWriteDefault(PrimitiveTypeValue value) {
	write_default_.emplace(std::move(value));
	return *this;
}

StructField StructFieldBuilder::Build() {
	if (!has_id_) {
		throw InvalidInputException("StructField required property 'id' is missing");
	}
	if (!has_name_) {
		throw InvalidInputException("StructField required property 'name' is missing");
	}
	if (!has_type_) {
		throw InvalidInputException("StructField required property 'type' is missing");
	}
	if (!has_required_) {
		throw InvalidInputException("StructField required property 'required' is missing");
	}
	auto result = StructField(std::move(*id_), std::move(*name_), std::move(type_), std::move(*required_),
	                          std::move(_doc_), std::move(initial_default_), std::move(write_default_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string StructFieldBuilder::TryBuild(optional<StructField> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string StructField::TryFromJSON(yyjson_val *obj, StructFieldBuilder &builder) {
	try {
		auto id_val = yyjson_obj_get(obj, "id");
		if (!id_val) {
			throw InvalidInputException("StructField required property 'id' is missing");
		} else {
			int32_t id;
			if (yyjson_is_int(id_val)) {
				id = yyjson_get_int(id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructField property 'id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(id_val)));
			}
			builder.SetId(std::move(id));
		}
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("StructField required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructField property 'name' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("StructField required property 'type' is missing");
		} else {
			unique_ptr<Type> type;
			type = make_uniq<Type>(Type::FromJSON(type_val));
			builder.SetType(std::move(type));
		}
		auto required_val = yyjson_obj_get(obj, "required");
		if (!required_val) {
			throw InvalidInputException("StructField required property 'required' is missing");
		} else {
			bool required;
			if (yyjson_is_bool(required_val)) {
				required = yyjson_get_bool(required_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructField property 'required' is not of type 'boolean', found '%s' instead",
				                       yyjson_get_type_desc(required_val)));
			}
			builder.SetRequired(std::move(required));
		}
		auto _doc_val = yyjson_obj_get(obj, "doc");
		if (_doc_val) {
			string _doc;
			if (yyjson_is_str(_doc_val)) {
				_doc = yyjson_get_str(_doc_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructField property '_doc' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(_doc_val)));
			}
			builder.SetDoc(std::move(_doc));
		}
		auto initial_default_val = yyjson_obj_get(obj, "initial-default");
		if (initial_default_val) {
			builder.SetInitialDefault(PrimitiveTypeValue::FromJSON(initial_default_val));
		}
		auto write_default_val = yyjson_obj_get(obj, "write-default");
		if (write_default_val) {
			builder.SetWriteDefault(PrimitiveTypeValue::FromJSON(write_default_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

StructField StructField::FromJSON(yyjson_val *obj) {
	StructFieldBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

StructField StructField::Copy() const {
	StructFieldBuilder builder;
	int32_t id_tmp;
	id_tmp = id;
	builder.SetId(std::move(id_tmp));
	string name_tmp;
	name_tmp = name;
	builder.SetName(std::move(name_tmp));
	unique_ptr<Type> type_tmp;
	type_tmp = type ? make_uniq<Type>(type->Copy()) : nullptr;
	builder.SetType(std::move(type_tmp));
	bool required_tmp;
	required_tmp = required;
	builder.SetRequired(std::move(required_tmp));
	optional<string> _doc_tmp;
	if (_doc.has_value()) {
		_doc_tmp.emplace();
		(*_doc_tmp) = (*_doc);
	}
	if (_doc_tmp.has_value()) {
		builder.SetDoc(std::move((*_doc_tmp)));
	}
	optional<PrimitiveTypeValue> initial_default_tmp;
	if (initial_default.has_value()) {
		initial_default_tmp.emplace((*initial_default).Copy());
	}
	if (initial_default_tmp.has_value()) {
		builder.SetInitialDefault(std::move(*initial_default_tmp));
	}
	optional<PrimitiveTypeValue> write_default_tmp;
	if (write_default.has_value()) {
		write_default_tmp.emplace((*write_default).Copy());
	}
	if (write_default_tmp.has_value()) {
		builder.SetWriteDefault(std::move(*write_default_tmp));
	}
	return builder.Build();
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
