
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

Schema::Schema()
    : struct_type(GeneratedObjectAccess::Create<StructType>()), object_1(GeneratedObjectAccess::Create<Object1>()) {
}
Schema::Object1::Object1() {
}

Schema::Object1Builder::Object1Builder() {
}

Schema::Object1Builder &Schema::Object1Builder::SetSchemaId(int32_t value) {
	result_.schema_id = std::move(value);
	return *this;
}

Schema::Object1Builder &Schema::Object1Builder::SetIdentifierFieldIds(vector<int32_t> value) {
	result_.identifier_field_ids = std::move(value);
	return *this;
}

string Schema::Object1Builder::TryBuild(Schema::Object1 &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Schema::Object1 Schema::Object1Builder::Build() {
	Schema::Object1 result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Schema::Object1 Schema::Object1::FromJSON(yyjson_val *obj) {
	Object1 res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Schema::Object1 Schema::Object1::Copy() const {
	Object1 res;
	if (schema_id.has_value()) {
		res.schema_id.emplace();
		(*res.schema_id) = (*schema_id);
	}
	if (identifier_field_ids.has_value()) {
		res.identifier_field_ids.emplace();
		(*res.identifier_field_ids).reserve((*identifier_field_ids).size());
		for (auto &item : (*identifier_field_ids)) {
			(*res.identifier_field_ids).emplace_back(item);
		}
	}
	return res;
}

string Schema::Object1::Validate() const {
	string error;
	return "";
}

string Schema::Object1::TryFromJSON(yyjson_val *obj) {
	string error;
	auto schema_id_val = yyjson_obj_get(obj, "schema-id");
	if (schema_id_val) {
		int32_t schema_id_tmp;
		if (yyjson_is_int(schema_id_val)) {
			schema_id_tmp = yyjson_get_int(schema_id_val);
		} else {
			return StringUtil::Format("Object1 property 'schema_id_tmp' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(schema_id_val));
		}
		schema_id = std::move(schema_id_tmp);
	}
	auto identifier_field_ids_val = yyjson_obj_get(obj, "identifier-field-ids");
	if (identifier_field_ids_val) {
		vector<int32_t> identifier_field_ids_tmp;
		if (yyjson_is_arr(identifier_field_ids_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(identifier_field_ids_val, idx, max, val) {
				int32_t tmp;
				if (yyjson_is_int(val)) {
					tmp = yyjson_get_int(val);
				} else {
					return StringUtil::Format("Object1 property 'tmp' is not of type 'integer', found '%s' instead",
					                          yyjson_get_type_desc(val));
				}
				identifier_field_ids_tmp.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "Object1 property 'identifier_field_ids_tmp' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(identifier_field_ids_val));
		}
		identifier_field_ids = std::move(identifier_field_ids_tmp);
	}
	return Validate();
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
	result_.struct_type = std::move(value);
	return *this;
}

SchemaBuilder &SchemaBuilder::SetObject1(Schema::Object1 value) {
	result_.object_1 = std::move(value);
	return *this;
}

string SchemaBuilder::TryBuild(Schema &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Schema SchemaBuilder::Build() {
	Schema result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Schema Schema::FromJSON(yyjson_val *obj) {
	Schema res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Schema Schema::Copy() const {
	Schema res;
	res.struct_type = struct_type.Copy();
	res.object_1 = object_1.Copy();
	return res;
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

string Schema::TryFromJSON(yyjson_val *obj) {
	string error;
	error = struct_type.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	error = object_1.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	return Validate();
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
