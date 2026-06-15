
#include "rest_catalog/objects/partition_field.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

PartitionField::PartitionField() {
}

PartitionFieldBuilder::PartitionFieldBuilder() {
}

PartitionFieldBuilder &PartitionFieldBuilder::SetSourceId(int32_t value) {
	result_.source_id = std::move(value);
	has_source_id_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetTransform(Transform value) {
	result_.transform = std::move(value);
	has_transform_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetName(string value) {
	result_.name = std::move(value);
	has_name_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetFieldId(int32_t value) {
	result_.field_id = std::move(value);
	return *this;
}

string PartitionFieldBuilder::TryBuild(PartitionField &result) {
	if (!has_source_id_) {
		return "PartitionField required property 'source-id' is missing";
	}
	if (!has_transform_) {
		return "PartitionField required property 'transform' is missing";
	}
	if (!has_name_) {
		return "PartitionField required property 'name' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

PartitionField PartitionFieldBuilder::Build() {
	PartitionField result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

PartitionField PartitionField::FromJSON(yyjson_val *obj) {
	PartitionField res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

PartitionField PartitionField::Copy() const {
	PartitionField res;
	res.source_id = source_id;
	res.transform = transform.Copy();
	res.name = name;
	if (field_id.has_value()) {
		res.field_id.emplace();
		(*res.field_id) = (*field_id);
	}
	return res;
}

string PartitionField::Validate() const {
	string error;
	error = transform.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string PartitionField::TryFromJSON(yyjson_val *obj) {
	string error;
	auto source_id_val = yyjson_obj_get(obj, "source-id");
	if (!source_id_val) {
		return "PartitionField required property 'source-id' is missing";
	} else {
		if (yyjson_is_int(source_id_val)) {
			source_id = yyjson_get_int(source_id_val);
		} else {
			return StringUtil::Format(
			    "PartitionField property 'source_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(source_id_val));
		}
	}
	auto transform_val = yyjson_obj_get(obj, "transform");
	if (!transform_val) {
		return "PartitionField required property 'transform' is missing";
	} else {
		error = transform.TryFromJSON(transform_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto name_val = yyjson_obj_get(obj, "name");
	if (!name_val) {
		return "PartitionField required property 'name' is missing";
	} else {
		if (yyjson_is_str(name_val)) {
			name = yyjson_get_str(name_val);
		} else {
			return StringUtil::Format("PartitionField property 'name' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(name_val));
		}
	}
	auto field_id_val = yyjson_obj_get(obj, "field-id");
	if (field_id_val) {
		int32_t field_id_tmp;
		if (yyjson_is_int(field_id_val)) {
			field_id_tmp = yyjson_get_int(field_id_val);
		} else {
			return StringUtil::Format(
			    "PartitionField property 'field_id_tmp' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(field_id_val));
		}
		field_id = std::move(field_id_tmp);
	}
	return Validate();
}

void PartitionField::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: source-id
	yyjson_mut_obj_add_int(doc, obj, "source-id", source_id);

	// Serialize: transform
	yyjson_mut_val *transform_val = transform.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "transform", transform_val);

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: field-id
	if (field_id.has_value()) {
		auto &field_id_value = *field_id;
		yyjson_mut_obj_add_int(doc, obj, "field-id", field_id_value);
	}
}

yyjson_mut_val *PartitionField::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
