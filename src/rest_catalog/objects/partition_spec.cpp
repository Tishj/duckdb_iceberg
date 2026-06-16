
#include "rest_catalog/objects/partition_spec.hpp"

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

PartitionSpec::PartitionSpec(vector<PartitionField> fields_p, optional<int32_t> spec_id_p)
    : fields(std::move(fields_p)), spec_id(std::move(spec_id_p)) {
}

PartitionSpecBuilder::PartitionSpecBuilder() {
}

PartitionSpecBuilder &PartitionSpecBuilder::SetFields(vector<PartitionField> value) {
	fields_.emplace(std::move(value));
	has_fields_ = true;
	return *this;
}

PartitionSpecBuilder &PartitionSpecBuilder::SetSpecId(int32_t value) {
	spec_id_.emplace(std::move(value));
	return *this;
}

PartitionSpec PartitionSpecBuilder::Build() {
	if (!has_fields_) {
		throw InvalidInputException("PartitionSpec required property 'fields' is missing");
	}
	auto result = PartitionSpec(std::move(*fields_), std::move(spec_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PartitionSpecBuilder::TryBuild(optional<PartitionSpec> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string PartitionSpec::TryFromJSON(yyjson_val *obj, PartitionSpecBuilder &builder) {
	try {
		auto fields_val = yyjson_obj_get(obj, "fields");
		if (!fields_val) {
			throw InvalidInputException("PartitionSpec required property 'fields' is missing");
		} else {
			vector<PartitionField> fields;
			if (yyjson_is_arr(fields_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(fields_val, idx, max, val) {
					auto tmp = PartitionField::FromJSON(val);
					fields.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("PartitionSpec property 'fields' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(fields_val)));
			}
			builder.SetFields(std::move(fields));
		}
		auto spec_id_val = yyjson_obj_get(obj, "spec-id");
		if (spec_id_val) {
			int32_t spec_id;
			if (yyjson_is_int(spec_id_val)) {
				spec_id = yyjson_get_int(spec_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("PartitionSpec property 'spec_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(spec_id_val)));
			}
			builder.SetSpecId(std::move(spec_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PartitionSpec PartitionSpec::FromJSON(yyjson_val *obj) {
	PartitionSpecBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

PartitionSpec PartitionSpec::Copy() const {
	PartitionSpecBuilder builder;
	vector<PartitionField> fields_tmp;
	fields_tmp.reserve(fields.size());
	for (auto &item : fields) {
		fields_tmp.emplace_back(item.Copy());
	}
	builder.SetFields(std::move(fields_tmp));
	optional<int32_t> spec_id_tmp;
	if (spec_id.has_value()) {
		spec_id_tmp.emplace();
		(*spec_id_tmp) = (*spec_id);
	}
	if (spec_id_tmp.has_value()) {
		builder.SetSpecId(std::move((*spec_id_tmp)));
	}
	return builder.Build();
}

string PartitionSpec::Validate() const {
	string error;
	for (const auto &item : fields) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void PartitionSpec::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: fields
	yyjson_mut_val *fields_arr = yyjson_mut_arr(doc);
	for (const auto &item : fields) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(fields_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "fields", fields_arr);

	// Serialize: spec-id
	if (spec_id.has_value()) {
		auto &spec_id_value = *spec_id;
		yyjson_mut_obj_add_int(doc, obj, "spec-id", spec_id_value);
	}
}

yyjson_mut_val *PartitionSpec::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
