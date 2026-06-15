
#include "rest_catalog/objects/schema.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Schema::Schema(StructType struct_type_p, Object1 object_1_p)
    : struct_type(std::move(struct_type_p)), object_1(std::move(object_1_p)) {
}
Schema::Object1::Object1(optional<int32_t> schema_id_p, optional<vector<int32_t>> identifier_field_ids_p)
    : schema_id(std::move(schema_id_p)), identifier_field_ids(std::move(identifier_field_ids_p)) {
}

Schema::Object1Builder::Object1Builder() {
}

Schema::Object1Builder &Schema::Object1Builder::SetSchemaId(int32_t value) {
	schema_id_ = std::move(value);
	return *this;
}

Schema::Object1Builder &Schema::Object1Builder::SetIdentifierFieldIds(vector<int32_t> value) {
	identifier_field_ids_ = std::move(value);
	return *this;
}

Schema::Object1 Schema::Object1Builder::Build() {
	auto result = Schema::Object1(std::move(schema_id_), std::move(identifier_field_ids_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string Schema::Object1Builder::TryBuild(optional<Schema::Object1> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Schema::Object1 Schema::Object1::FromJSON(yyjson_val *obj) {
	Object1Builder builder;
	auto schema_id_val = yyjson_obj_get(obj, "schema-id");
	if (schema_id_val) {
		int32_t schema_id;
		if (yyjson_is_int(schema_id_val)) {
			schema_id = yyjson_get_int(schema_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("Object1 property 'schema_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(schema_id_val)));
		}
		builder.SetSchemaId(std::move(schema_id));
	}
	auto identifier_field_ids_val = yyjson_obj_get(obj, "identifier-field-ids");
	if (identifier_field_ids_val) {
		vector<int32_t> identifier_field_ids;
		if (yyjson_is_arr(identifier_field_ids_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(identifier_field_ids_val, idx, max, val) {
				int32_t tmp;
				if (yyjson_is_int(val)) {
					tmp = yyjson_get_int(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("Object1 property 'tmp' is not of type 'integer', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				identifier_field_ids.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "Object1 property 'identifier_field_ids' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(identifier_field_ids_val));
		}
		builder.SetIdentifierFieldIds(std::move(identifier_field_ids));
	}
	return builder.Build();
}

string Schema::Object1::TryFromJSON(yyjson_val *obj, optional<Schema::Object1> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Schema::Object1 Schema::Object1::Copy() const {
	Object1Builder builder;
	int32_t schema_id_tmp;
	if (schema_id.has_value()) {
		schema_id_tmp.emplace();
		(*schema_id_tmp) = (*schema_id);
	}
	if (schema_id_tmp.has_value()) {
		builder.SetSchemaId(std::move(schema_id_tmp));
	}
	vector<int32_t> identifier_field_ids_tmp;
	if (identifier_field_ids.has_value()) {
		identifier_field_ids_tmp.emplace();
		(*identifier_field_ids_tmp).reserve((*identifier_field_ids).size());
		for (auto &item : (*identifier_field_ids)) {
			(*identifier_field_ids_tmp).emplace_back(item);
		}
	}
	if (identifier_field_ids_tmp.has_value()) {
		builder.SetIdentifierFieldIds(std::move(identifier_field_ids_tmp));
	}
	return builder.Build();
}

string Schema::Object1::Validate() const {
	string error;
	return "";
}

void Schema::Object1::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: schema-id
	if (schema_id.has_value()) {
		auto &schema_id_value = *schema_id;
		yyjson_mut_obj_add_int(doc, obj, "schema-id", schema_id_value);
	}

	// Serialize: identifier-field-ids
	if (identifier_field_ids.has_value()) {
		auto &identifier_field_ids_value = *identifier_field_ids;
		yyjson_mut_val *identifier_field_ids_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : identifier_field_ids_value) {
			yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
			yyjson_mut_arr_append(identifier_field_ids_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "identifier-field-ids", identifier_field_ids_value_arr);
	}
}

yyjson_mut_val *Schema::Object1::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

SchemaBuilder::SchemaBuilder() {
}

SchemaBuilder &SchemaBuilder::SetStructType(StructType value) {
	struct_type_ = std::move(value);
	return *this;
}

SchemaBuilder &SchemaBuilder::SetObject1(Schema::Object1 value) {
	object_1_ = std::move(value);
	return *this;
}

Schema SchemaBuilder::Build() {
	auto result = Schema(std::move(*struct_type_), std::move(*object_1_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SchemaBuilder::TryBuild(optional<Schema> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Schema Schema::FromJSON(yyjson_val *obj) {
	SchemaBuilder builder;
	builder.SetStructType(StructType::FromJSON(obj));
	builder.SetObject1(Object1::FromJSON(obj));
	return builder.Build();
}

string Schema::TryFromJSON(yyjson_val *obj, optional<Schema> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Schema Schema::Copy() const {
	SchemaBuilder builder;
	optional<StructType> struct_type_tmp;
	struct_type_tmp = struct_type.Copy();
	builder.SetStructType(std::move(*struct_type_tmp));
	optional<Object1> object_1_tmp;
	object_1_tmp = object_1.Copy();
	builder.SetObject1(std::move(*object_1_tmp));
	return builder.Build();
}

string Schema::Validate() const {
	string error;
	error = struct_type.Validate();
	if (!error.empty()) {
		return error;
	}
	error = object_1.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void Schema::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: StructType
	struct_type.PopulateJSON(doc, obj);

	// Serialize base class: Object1
	object_1.PopulateJSON(doc, obj);
}

yyjson_mut_val *Schema::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
