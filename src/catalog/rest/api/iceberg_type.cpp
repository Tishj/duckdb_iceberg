#include "catalog/rest/api/iceberg_type.hpp"
#include "common/iceberg_constants.hpp"

#include "duckdb/common/string_util.hpp"
#include "duckdb/common/extra_type_info.hpp"
#include "duckdb/common/types.hpp"
#include "duckdb/common/types/blob.hpp"
#include "duckdb/parser/column_definition.hpp"

#include "rest_catalog/objects/list_type.hpp"
#include "rest_catalog/objects/map_type.hpp"
#include "rest_catalog/objects/struct_type.hpp"
#include "rest_catalog/objects/struct_field.hpp"
#include "rest_catalog/objects/type.hpp"

namespace duckdb {

static string ConvertBlobDefault(const string_t &str) {
	string result;
	result.resize(str.GetSize() * 2);
	idx_t str_idx = 0;
	auto data = str.GetData();
	auto len = str.GetSize();
	for (idx_t i = 0; i < len; i++) {
		auto byte_a = (data[i] >> 4) & 0x0F;
		auto byte_b = data[i] & 0x0F;
		D_ASSERT(byte_a >= 0 && byte_a < 16);
		D_ASSERT(byte_b >= 0 && byte_b < 16);
		// non-ascii characters are rendered as hexadecimal (e.g. \x00)
		result[str_idx++] = Blob::HEX_TABLE[byte_a];
		result[str_idx++] = Blob::HEX_TABLE[byte_b];
	}
	return result;
}

string IcebergTypeHelper::LogicalTypeToIcebergType(const LogicalType &type) {
	switch (type.id()) {
	case LogicalTypeId::INTEGER:
		return "int";
	case LogicalTypeId::BOOLEAN:
		return "boolean";
	case LogicalTypeId::VARCHAR: {
		if (type.IsJSONType()) {
			throw InvalidTypeException("Type JSON is not a valid Iceberg Type");
		}
		return "string";
	}
	case LogicalTypeId::DATE:
		return "date";
	case LogicalTypeId::BIGINT:
		return "long";
	case LogicalTypeId::HUGEINT:
		// Iceberg doesnt have native 128bit int, decimal(38,0) covers the range
		return "decimal(38, 0)";
	case LogicalTypeId::FLOAT:
		return "float";
	case LogicalTypeId::DOUBLE:
		return "double";
	case LogicalTypeId::DECIMAL: {
		auto width = DecimalType::GetWidth(type);
		auto scale = DecimalType::GetScale(type);
		return StringUtil::Format("decimal(%d, %d)", width, scale);
	}
	case LogicalTypeId::UUID:
		return "uuid";
	case LogicalTypeId::BLOB:
		return "binary";
	case LogicalTypeId::STRUCT:
		return "struct";
	case LogicalTypeId::LIST:
		// Iceberg doesn't support fixed array lengths
		return "list";
	case LogicalTypeId::TIME:
		return "time";
	case LogicalTypeId::TIMESTAMP:
		return "timestamp";
	case LogicalTypeId::TIMESTAMP_TZ:
		return "timestamptz";
	case LogicalTypeId::TIMESTAMP_NS:
		return "timestamp_ns";
	case LogicalTypeId::MAP:
		return "map";
	case LogicalTypeId::VARIANT:
		return "variant";
	case LogicalTypeId::GEOMETRY: {
		if (GeoType::HasCRS(type)) {
			return StringUtil::Format("geometry(%s)", GeoType::GetCRS(type).GetIdentifier());
		}
		// use default coordinate system
		return "geometry(" + StringUtil::Lower(IcebergConstants::DefaultGeometryCRS) + ")";
	}
	default:
		throw InvalidInputException("Column type %s is not a valid Iceberg Type.", LogicalTypeIdToString(type.id()));
	}
}

yyjson_mut_val *IcebergTypeHelper::PrimitiveTypeValueToJSON(yyjson_mut_doc *doc,
                                                            const rest_api_objects::PrimitiveTypeValue &value) {
	if (value.boolean_type_value) {
		return yyjson_mut_bool(doc, value.boolean_type_value->value);
	} else if (value.long_type_value) {
		return yyjson_mut_int(doc, value.long_type_value->value);
	} else if (value.integer_type_value) {
		return yyjson_mut_int(doc, value.integer_type_value->value);
	} else if (value.double_type_value) {
		return yyjson_mut_real(doc, value.double_type_value->value);
	} else if (value.float_type_value) {
		return yyjson_mut_real(doc, value.float_type_value->value);
	} else if (value.decimal_type_value) {
		auto &str = value.decimal_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.string_type_value) {
		auto &str = value.string_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.uuidtype_value) {
		auto &str = value.uuidtype_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.date_type_value) {
		auto &str = value.date_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.time_type_value) {
		auto &str = value.time_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.timestamp_type_value) {
		auto &str = value.timestamp_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.timestamp_tz_type_value) {
		auto &str = value.timestamp_tz_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.timestamp_nano_type_value) {
		auto &str = value.timestamp_nano_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.timestamp_tz_nano_type_value) {
		auto &str = value.timestamp_tz_nano_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.fixed_type_value) {
		auto &str = value.fixed_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else if (value.binary_type_value) {
		auto &str = value.binary_type_value->value;
		return yyjson_mut_strncpy(doc, str.c_str(), str.size());
	} else {
		return yyjson_mut_null(doc);
	}
}

rest_api_objects::PrimitiveTypeValue IcebergTypeHelper::PrimitiveTypeFromValue(const Value &value) {
	if (value.IsNull()) {
		throw InternalException("Can't produce a PrimitiveTypeValue from NULL");
	}
	auto &type = value.type();
	switch (type.id()) {
	case LogicalTypeId::VARIANT: {
		throw NotImplementedException("DEFAULT values for VARIANT are not supported yet");
	}
	//! BooleanTypeValue
	case LogicalTypeId::BOOLEAN: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetBooleanTypeValue(rest_api_objects::BooleanTypeValue(value.GetValue<bool>()))
		    .Build();
	}
	case LogicalTypeId::INTEGER: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetIntegerTypeValue(rest_api_objects::IntegerTypeValue(value.GetValue<int32_t>()))
		    .Build();
	}
	case LogicalTypeId::BIGINT: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetLongTypeValue(rest_api_objects::LongTypeValue(value.GetValue<int64_t>()))
		    .Build();
	}
	case LogicalTypeId::FLOAT: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetFloatTypeValue(rest_api_objects::FloatTypeValue(value.GetValue<float>()))
		    .Build();
	}
	case LogicalTypeId::DOUBLE: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetDoubleTypeValue(rest_api_objects::DoubleTypeValue(value.GetValue<double>()))
		    .Build();
	}
	//! DecimalTypeValue
	case LogicalTypeId::DECIMAL: {
		//! FIXME: Spec says scientific notation should be used for negative scale decimals
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetStringTypeValue(rest_api_objects::StringTypeValue(value.ToString()))
		    .Build();
	}
	//! NOTE: when parsing we can't differentiate between these, so we set string_type
	//! StringTypeValue
	//! UUIDTypeValue
	//! DateTypeValue
	//! TimeTypeValue
	case LogicalTypeId::UUID:
	case LogicalTypeId::DATE:
	case LogicalTypeId::TIME:
	case LogicalTypeId::VARCHAR: {
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetStringTypeValue(rest_api_objects::StringTypeValue(value.ToString()))
		    .Build();
	}
	//! TimestampTypeValue
	//! TimestampNanoTypeValue
	//! TimestampTzTypeValue
	//! TimestampTzNanoTypeValue
	case LogicalTypeId::TIMESTAMP:
	case LogicalTypeId::TIMESTAMP_NS:
	case LogicalTypeId::TIMESTAMP_TZ:
	case LogicalTypeId::TIMESTAMP_TZ_NS: {
		auto raw = value.ToString();
		auto splits = StringUtil::Split(raw, ' ');
		D_ASSERT(splits.size() == 2);
		auto str = StringUtil::Join(splits, "T");

		if (type.id() == LogicalTypeId::TIMESTAMP || type.id() == LogicalTypeId::TIMESTAMP_NS) {
			return rest_api_objects::PrimitiveTypeValueBuilder()
			    .SetStringTypeValue(rest_api_objects::StringTypeValue(str))
			    .Build();
		}

		str += ":00";
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetStringTypeValue(rest_api_objects::StringTypeValue(str))
		    .Build();
	}
	//! FIXME: missing FixedTypeValue
	//! BinaryTypeValue
	case LogicalTypeId::BLOB: {
		auto str = value.GetValueUnsafe<string_t>();
		auto blob_str = ConvertBlobDefault(str);
		return rest_api_objects::PrimitiveTypeValueBuilder()
		    .SetBinaryTypeValue(rest_api_objects::BinaryTypeValue(blob_str))
		    .Build();
	}
	default:
		throw NotImplementedException("DEFAULT values for nested types (like %s) not implemented", type.ToString());
	}
}

rest_api_objects::StructField IcebergTypeHelper::CreateIcebergRestType(const string &name, const LogicalType &type,
                                                                       bool required, const string &doc,
                                                                       const Value &default_val,
                                                                       const std::function<idx_t()> &get_next_id) {
	auto field_id = static_cast<int32_t>(get_next_id());
	unique_ptr<rest_api_objects::Type> rest_type;
	optional<rest_api_objects::PrimitiveTypeValue> initial_default;
	if (!default_val.IsNull() && type.id() != LogicalTypeId::STRUCT) {
		initial_default.emplace(IcebergTypeHelper::PrimitiveTypeFromValue(default_val));
	}

	switch (type.id()) {
	case LogicalTypeId::MAP: {
		//! Key
		auto key_type = MapType::KeyType(type);
		auto key_field = IcebergTypeHelper::CreateIcebergRestType("key", key_type, true, "",
		                                                          Value(), //! FIXME: extract from parent
		                                                          get_next_id);

		//! Value
		auto value_type = MapType::ValueType(type);
		auto value_field =
		    IcebergTypeHelper::CreateIcebergRestType("value", value_type, false, "", Value(), get_next_id);
		auto map_type =
		    rest_api_objects::MapTypeBuilder()
		        .SetType("map")
		        .SetKeyId(key_field.id)
		        .SetKey(key_field.type ? make_uniq<rest_api_objects::Type>(key_field.type->Copy()) : nullptr)
		        .SetValueId(value_field.id)
		        .SetValue(value_field.type ? make_uniq<rest_api_objects::Type>(value_field.type->Copy()) : nullptr)
		        .SetValueRequired(value_field.required)
		        .Build();
		rest_type =
		    make_uniq<rest_api_objects::Type>(rest_api_objects::TypeBuilder().SetMapType(std::move(map_type)).Build());
		break;
	}
	case LogicalTypeId::STRUCT: {
		auto &children = StructType::GetChildTypes(type);
		vector<unique_ptr<rest_api_objects::StructField>> fields;

		for (idx_t i = 0; i < children.size(); i++) {
			auto &child = children[i];
			auto &child_name = child.first;
			auto &child_type = child.second;
			auto child_default = !default_val.IsNull() ? StructValue::GetChildren(default_val)[i] : Value();
			auto struct_child = make_uniq<rest_api_objects::StructField>(IcebergTypeHelper::CreateIcebergRestType(
			    child_name.GetIdentifierName(), child_type, false, "", child_default, get_next_id));
			fields.push_back(std::move(struct_child));
		}
		auto struct_type = rest_api_objects::StructTypeBuilder().SetType("struct").SetFields(std::move(fields)).Build();
		rest_type = make_uniq<rest_api_objects::Type>(
		    rest_api_objects::TypeBuilder().SetStructType(std::move(struct_type)).Build());
		break;
	}
	case LogicalTypeId::LIST: {
		//! Element
		const auto &element_type = ListType::GetChildType(type);
		auto element_field = IcebergTypeHelper::CreateIcebergRestType("element", element_type, false, "",
		                                                              Value(), //! FIXME: extract default from parent
		                                                              get_next_id);
		auto list_type =
		    rest_api_objects::ListTypeBuilder()
		        .SetType("list")
		        .SetElementId(element_field.id)
		        .SetElement(element_field.type ? make_uniq<rest_api_objects::Type>(element_field.type->Copy())
		                                       : nullptr)
		        .SetElementRequired(element_field.required)
		        .Build();
		rest_type = make_uniq<rest_api_objects::Type>(
		    rest_api_objects::TypeBuilder().SetListType(std::move(list_type)).Build());
		break;
	}
	case LogicalTypeId::ARRAY: {
		throw InvalidConfigurationException("Array type not supported in Iceberg type. Please cast to LIST");
	}
	default:
		rest_type = make_uniq<rest_api_objects::Type>(
		    rest_api_objects::TypeBuilder()
		        .SetPrimitiveType(rest_api_objects::PrimitiveType(IcebergTypeHelper::LogicalTypeToIcebergType(type)))
		        .Build());
		break;
	}

	rest_api_objects::StructFieldBuilder result_builder;
	result_builder.SetId(field_id).SetName(name).SetType(std::move(rest_type)).SetRequired(required);
	if (!doc.empty()) {
		result_builder.SetDoc(doc);
	}
	if (initial_default) {
		result_builder.SetInitialDefault(std::move(*initial_default));
	}
	return result_builder.Build();
}

} // namespace duckdb
