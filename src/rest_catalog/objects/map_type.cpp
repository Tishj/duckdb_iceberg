
#include "rest_catalog/objects/map_type.hpp"

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

MapType::MapType(string type_p, int32_t key_id_p, unique_ptr<Type> key_p, int32_t value_id_p, unique_ptr<Type> value_p,
                 bool value_required_p)
    : type(std::move(type_p)), key_id(std::move(key_id_p)), key(std::move(key_p)), value_id(std::move(value_id_p)),
      value(std::move(value_p)), value_required(std::move(value_required_p)) {
}

MapTypeBuilder::MapTypeBuilder() {
}

MapTypeBuilder &MapTypeBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

MapTypeBuilder &MapTypeBuilder::SetKeyId(int32_t value) {
	key_id_.emplace(std::move(value));
	has_key_id_ = true;
	return *this;
}

MapTypeBuilder &MapTypeBuilder::SetKey(unique_ptr<Type> value) {
	key_ = std::move(value);
	has_key_ = true;
	return *this;
}

MapTypeBuilder &MapTypeBuilder::SetValueId(int32_t value) {
	value_id_.emplace(std::move(value));
	has_value_id_ = true;
	return *this;
}

MapTypeBuilder &MapTypeBuilder::SetValue(unique_ptr<Type> value) {
	value_ = std::move(value);
	has_value_ = true;
	return *this;
}

MapTypeBuilder &MapTypeBuilder::SetValueRequired(bool value) {
	value_required_.emplace(std::move(value));
	has_value_required_ = true;
	return *this;
}

MapType MapTypeBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("MapType required property 'type' is missing");
	}
	if (!has_key_id_) {
		throw InvalidInputException("MapType required property 'key-id' is missing");
	}
	if (!has_key_) {
		throw InvalidInputException("MapType required property 'key' is missing");
	}
	if (!has_value_id_) {
		throw InvalidInputException("MapType required property 'value-id' is missing");
	}
	if (!has_value_) {
		throw InvalidInputException("MapType required property 'value' is missing");
	}
	if (!has_value_required_) {
		throw InvalidInputException("MapType required property 'value-required' is missing");
	}
	auto result = MapType(std::move(*type_), std::move(*key_id_), std::move(key_), std::move(*value_id_),
	                      std::move(value_), std::move(*value_required_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string MapTypeBuilder::TryBuild(optional<MapType> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MapType MapType::FromJSON(yyjson_val *obj) {
	MapTypeBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("MapType required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "MapType property 'type' is not of type 'string', found '%s' instead", yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto key_id_val = yyjson_obj_get(obj, "key-id");
	if (!key_id_val) {
		throw InvalidInputException("MapType required property 'key-id' is missing");
	} else {
		int32_t key_id;
		if (yyjson_is_int(key_id_val)) {
			key_id = yyjson_get_int(key_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("MapType property 'key_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(key_id_val)));
		}
		builder.SetKeyId(std::move(key_id));
	}
	auto key_val = yyjson_obj_get(obj, "key");
	if (!key_val) {
		throw InvalidInputException("MapType required property 'key' is missing");
	} else {
		unique_ptr<Type> key;
		key = make_uniq<Type>(Type::FromJSON(key_val));
		builder.SetKey(std::move(key));
	}
	auto value_id_val = yyjson_obj_get(obj, "value-id");
	if (!value_id_val) {
		throw InvalidInputException("MapType required property 'value-id' is missing");
	} else {
		int32_t value_id;
		if (yyjson_is_int(value_id_val)) {
			value_id = yyjson_get_int(value_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("MapType property 'value_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(value_id_val)));
		}
		builder.SetValueId(std::move(value_id));
	}
	auto value_val = yyjson_obj_get(obj, "value");
	if (!value_val) {
		throw InvalidInputException("MapType required property 'value' is missing");
	} else {
		unique_ptr<Type> value;
		value = make_uniq<Type>(Type::FromJSON(value_val));
		builder.SetValue(std::move(value));
	}
	auto value_required_val = yyjson_obj_get(obj, "value-required");
	if (!value_required_val) {
		throw InvalidInputException("MapType required property 'value-required' is missing");
	} else {
		bool value_required;
		if (yyjson_is_bool(value_required_val)) {
			value_required = yyjson_get_bool(value_required_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("MapType property 'value_required' is not of type 'boolean', found '%s' instead",
			                       yyjson_get_type_desc(value_required_val)));
		}
		builder.SetValueRequired(std::move(value_required));
	}
	return builder.Build();
}

string MapType::TryFromJSON(yyjson_val *obj, optional<MapType> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MapType MapType::Copy() const {
	MapTypeBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	int32_t key_id_tmp;
	key_id_tmp = key_id;
	builder.SetKeyId(std::move(key_id_tmp));
	unique_ptr<Type> key_tmp;
	key_tmp = key ? make_uniq<Type>(key->Copy()) : nullptr;
	builder.SetKey(std::move(key_tmp));
	int32_t value_id_tmp;
	value_id_tmp = value_id;
	builder.SetValueId(std::move(value_id_tmp));
	unique_ptr<Type> value_tmp;
	value_tmp = value ? make_uniq<Type>(value->Copy()) : nullptr;
	builder.SetValue(std::move(value_tmp));
	bool value_required_tmp;
	value_required_tmp = value_required;
	builder.SetValueRequired(std::move(value_required_tmp));
	return builder.Build();
}

string MapType::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "map")) {
		return StringUtil::Format("MapType property 'type' must be map, not %s", type);
	}
	error = key->Validate();
	if (!error.empty()) {
		return error;
	}
	error = value->Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void MapType::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: key-id
	yyjson_mut_obj_add_int(doc, obj, "key-id", key_id);

	// Serialize: key
	yyjson_mut_val *key_val = key->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "key", key_val);

	// Serialize: value-id
	yyjson_mut_obj_add_int(doc, obj, "value-id", value_id);

	// Serialize: value
	yyjson_mut_val *value_val = value->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "value", value_val);

	// Serialize: value-required
	yyjson_mut_obj_add_bool(doc, obj, "value-required", value_required);
}

yyjson_mut_val *MapType::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
