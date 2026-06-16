
#include "rest_catalog/objects/blob_metadata.hpp"

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

BlobMetadata::BlobMetadata(string type_p, int64_t snapshot_id_p, int64_t sequence_number_p, vector<int32_t> fields_p,
                           optional<case_insensitive_map_t<string>> properties_p)
    : type(std::move(type_p)), snapshot_id(std::move(snapshot_id_p)), sequence_number(std::move(sequence_number_p)),
      fields(std::move(fields_p)), properties(std::move(properties_p)) {
}
BlobMetadata::BlobMetadata(const BlobMetadata &other)
    : type(other.type), snapshot_id(other.snapshot_id), sequence_number(other.sequence_number), fields(([&]() {
	      vector<int32_t> copied;
	      copied.reserve(other.fields.size());
	      for (const auto &item : other.fields) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())),
      properties((other.properties.has_value() ? optional<case_insensitive_map_t<string>>(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : (*other.properties)) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }()))
                                               : optional<case_insensitive_map_t<string>>())) {
}
BlobMetadata::BlobMetadata(BlobMetadata &&other) : BlobMetadata(static_cast<const BlobMetadata &>(other)) {
}

BlobMetadataBuilder::BlobMetadataBuilder() {
}

BlobMetadataBuilder &BlobMetadataBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

BlobMetadataBuilder &BlobMetadataBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

BlobMetadataBuilder &BlobMetadataBuilder::SetSequenceNumber(int64_t value) {
	sequence_number_.emplace(std::move(value));
	has_sequence_number_ = true;
	return *this;
}

BlobMetadataBuilder &BlobMetadataBuilder::SetFields(vector<int32_t> value) {
	fields_.emplace(std::move(value));
	has_fields_ = true;
	return *this;
}

BlobMetadataBuilder &BlobMetadataBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

BlobMetadata BlobMetadataBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("BlobMetadata required property 'type' is missing");
	}
	if (!has_snapshot_id_) {
		throw InvalidInputException("BlobMetadata required property 'snapshot-id' is missing");
	}
	if (!has_sequence_number_) {
		throw InvalidInputException("BlobMetadata required property 'sequence-number' is missing");
	}
	if (!has_fields_) {
		throw InvalidInputException("BlobMetadata required property 'fields' is missing");
	}
	auto result = BlobMetadata(std::move(*type_), std::move(*snapshot_id_), std::move(*sequence_number_),
	                           std::move(*fields_), std::move(properties_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string BlobMetadataBuilder::TryBuild(optional<BlobMetadata> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string BlobMetadata::TryFromJSON(yyjson_val *obj, BlobMetadataBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("BlobMetadata required property 'type' is missing");
		} else {
			string type;
			if (yyjson_is_str(type_val)) {
				type = yyjson_get_str(type_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("BlobMetadata property 'type' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(type_val)));
			}
			builder.SetType(std::move(type));
		}
		auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
		if (!snapshot_id_val) {
			throw InvalidInputException("BlobMetadata required property 'snapshot-id' is missing");
		} else {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "BlobMetadata property 'snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
		auto sequence_number_val = yyjson_obj_get(obj, "sequence-number");
		if (!sequence_number_val) {
			throw InvalidInputException("BlobMetadata required property 'sequence-number' is missing");
		} else {
			int64_t sequence_number;
			if (yyjson_is_sint(sequence_number_val)) {
				sequence_number = yyjson_get_sint(sequence_number_val);
			} else if (yyjson_is_uint(sequence_number_val)) {
				sequence_number = yyjson_get_uint(sequence_number_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "BlobMetadata property 'sequence_number' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(sequence_number_val)));
			}
			builder.SetSequenceNumber(std::move(sequence_number));
		}
		auto fields_val = yyjson_obj_get(obj, "fields");
		if (!fields_val) {
			throw InvalidInputException("BlobMetadata required property 'fields' is missing");
		} else {
			vector<int32_t> fields;
			if (yyjson_is_arr(fields_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(fields_val, idx, max, val) {
					int32_t tmp;
					if (yyjson_is_int(val)) {
						tmp = yyjson_get_int(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "BlobMetadata property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					fields.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("BlobMetadata property 'fields' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(fields_val)));
			}
			builder.SetFields(std::move(fields));
		}
		auto properties_val = yyjson_obj_get(obj, "properties");
		if (properties_val) {
			case_insensitive_map_t<string> properties;
			if (yyjson_is_obj(properties_val)) {
				size_t idx, max;
				yyjson_val *key, *val;
				yyjson_obj_foreach(properties_val, idx, max, key, val) {
					auto key_str = yyjson_get_str(key);
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "BlobMetadata property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("BlobMetadata property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

BlobMetadata BlobMetadata::FromJSON(yyjson_val *obj) {
	BlobMetadataBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

BlobMetadata BlobMetadata::Copy() const {
	return BlobMetadata(*this);
}

string BlobMetadata::Validate() const {
	string error;
	return "";
}

void BlobMetadata::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: sequence-number
	yyjson_mut_obj_add_sint(doc, obj, "sequence-number", sequence_number);

	// Serialize: fields
	yyjson_mut_val *fields_arr = yyjson_mut_arr(doc);
	for (const auto &item : fields) {
		yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
		yyjson_mut_arr_append(fields_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "fields", fields_arr);

	// Serialize: properties
	if (properties.has_value()) {
		auto &properties_value = *properties;
		yyjson_mut_val *properties_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : properties_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, properties_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "properties", properties_value_obj);
	}
}

yyjson_mut_val *BlobMetadata::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
