
#include "rest_catalog/objects/partition_field.hpp"

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

PartitionField::PartitionField(int32_t source_id_p, Transform transform_p, string name_p, optional<int32_t> field_id_p)
    : source_id(std::move(source_id_p)), transform(std::move(transform_p)), name(std::move(name_p)),
      field_id(std::move(field_id_p)) {
}
PartitionField::PartitionField(const PartitionField &other)
    : source_id(other.source_id), transform(other.transform.Copy()), name(other.name),
      field_id((other.field_id.has_value() ? optional<int32_t>((*other.field_id)) : optional<int32_t>())) {
}
PartitionField::PartitionField(PartitionField &&other) : PartitionField(static_cast<const PartitionField &>(other)) {
}

PartitionFieldBuilder::PartitionFieldBuilder() {
}

PartitionFieldBuilder &PartitionFieldBuilder::SetSourceId(int32_t value) {
	source_id_.emplace(std::move(value));
	has_source_id_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetTransform(Transform value) {
	transform_.emplace(std::move(value));
	has_transform_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

PartitionFieldBuilder &PartitionFieldBuilder::SetFieldId(int32_t value) {
	field_id_.emplace(std::move(value));
	return *this;
}

PartitionField PartitionFieldBuilder::Build() {
	if (!has_source_id_) {
		throw InvalidInputException("PartitionField required property 'source-id' is missing");
	}
	if (!has_transform_) {
		throw InvalidInputException("PartitionField required property 'transform' is missing");
	}
	if (!has_name_) {
		throw InvalidInputException("PartitionField required property 'name' is missing");
	}
	auto result =
	    PartitionField(std::move(*source_id_), std::move(*transform_), std::move(*name_), std::move(field_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PartitionFieldBuilder::TryBuild(optional<PartitionField> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string PartitionField::TryFromJSON(yyjson_val *obj, PartitionFieldBuilder &builder) {
	try {
		auto source_id_val = yyjson_obj_get(obj, "source-id");
		if (!source_id_val) {
			throw InvalidInputException("PartitionField required property 'source-id' is missing");
		} else {
			int32_t source_id;
			if (yyjson_is_int(source_id_val)) {
				source_id = yyjson_get_int(source_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PartitionField property 'source_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(source_id_val)));
			}
			builder.SetSourceId(std::move(source_id));
		}
		auto transform_val = yyjson_obj_get(obj, "transform");
		if (!transform_val) {
			throw InvalidInputException("PartitionField required property 'transform' is missing");
		} else {
			builder.SetTransform(Transform::FromJSON(transform_val));
		}
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("PartitionField required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("PartitionField property 'name' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		auto field_id_val = yyjson_obj_get(obj, "field-id");
		if (field_id_val) {
			int32_t field_id;
			if (yyjson_is_int(field_id_val)) {
				field_id = yyjson_get_int(field_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PartitionField property 'field_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(field_id_val)));
			}
			builder.SetFieldId(std::move(field_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PartitionField PartitionField::FromJSON(yyjson_val *obj) {
	PartitionFieldBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

PartitionField PartitionField::Copy() const {
	return PartitionField(*this);
}

string PartitionField::Validate() const {
	string error;
	error = transform.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
