
#include "rest_catalog/objects/create_table_request.hpp"

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

CreateTableRequest::CreateTableRequest(string name_p, Schema schema_p, optional<string> location_p,
                                       optional<PartitionSpec> partition_spec_p, optional<SortOrder> write_order_p,
                                       optional<bool> stage_create_p,
                                       optional<case_insensitive_map_t<string>> properties_p)
    : name(std::move(name_p)), schema(std::move(schema_p)), location(std::move(location_p)),
      partition_spec(std::move(partition_spec_p)), write_order(std::move(write_order_p)),
      stage_create(std::move(stage_create_p)), properties(std::move(properties_p)) {
}

CreateTableRequestBuilder::CreateTableRequestBuilder() {
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetSchema(Schema value) {
	schema_.emplace(std::move(value));
	has_schema_ = true;
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetLocation(string value) {
	location_.emplace(std::move(value));
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetPartitionSpec(PartitionSpec value) {
	partition_spec_.emplace(std::move(value));
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetWriteOrder(SortOrder value) {
	write_order_.emplace(std::move(value));
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetStageCreate(bool value) {
	stage_create_.emplace(std::move(value));
	return *this;
}

CreateTableRequestBuilder &CreateTableRequestBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

CreateTableRequest CreateTableRequestBuilder::Build() {
	if (!has_name_) {
		throw InvalidInputException("CreateTableRequest required property 'name' is missing");
	}
	if (!has_schema_) {
		throw InvalidInputException("CreateTableRequest required property 'schema' is missing");
	}
	auto result =
	    CreateTableRequest(std::move(*name_), std::move(*schema_), std::move(location_), std::move(partition_spec_),
	                       std::move(write_order_), std::move(stage_create_), std::move(properties_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CreateTableRequestBuilder::TryBuild(optional<CreateTableRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CreateTableRequest::TryFromJSON(yyjson_val *obj, CreateTableRequestBuilder &builder) {
	try {
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("CreateTableRequest required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("CreateTableRequest property 'name' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		auto schema_val = yyjson_obj_get(obj, "schema");
		if (!schema_val) {
			throw InvalidInputException("CreateTableRequest required property 'schema' is missing");
		} else {
			builder.SetSchema(Schema::FromJSON(schema_val));
		}
		auto location_val = yyjson_obj_get(obj, "location");
		if (location_val) {
			string location;
			if (yyjson_is_str(location_val)) {
				location = yyjson_get_str(location_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "CreateTableRequest property 'location' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(location_val)));
			}
			builder.SetLocation(std::move(location));
		}
		auto partition_spec_val = yyjson_obj_get(obj, "partition-spec");
		if (partition_spec_val) {
			builder.SetPartitionSpec(PartitionSpec::FromJSON(partition_spec_val));
		}
		auto write_order_val = yyjson_obj_get(obj, "write-order");
		if (write_order_val) {
			builder.SetWriteOrder(SortOrder::FromJSON(write_order_val));
		}
		auto stage_create_val = yyjson_obj_get(obj, "stage-create");
		if (stage_create_val) {
			bool stage_create;
			if (yyjson_is_bool(stage_create_val)) {
				stage_create = yyjson_get_bool(stage_create_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "CreateTableRequest property 'stage_create' is not of type 'boolean', found '%s' instead",
				    yyjson_get_type_desc(stage_create_val)));
			}
			builder.SetStageCreate(std::move(stage_create));
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
						    "CreateTableRequest property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("CreateTableRequest property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CreateTableRequest CreateTableRequest::FromJSON(yyjson_val *obj) {
	CreateTableRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CreateTableRequest CreateTableRequest::Copy() const {
	CreateTableRequestBuilder builder;
	string name_tmp;
	name_tmp = name;
	builder.SetName(std::move(name_tmp));
	auto schema_tmp = schema.Copy();
	builder.SetSchema(std::move(schema_tmp));
	optional<string> location_tmp;
	if (location.has_value()) {
		location_tmp.emplace();
		(*location_tmp) = (*location);
	}
	if (location_tmp.has_value()) {
		builder.SetLocation(std::move((*location_tmp)));
	}
	optional<PartitionSpec> partition_spec_tmp;
	if (partition_spec.has_value()) {
		partition_spec_tmp.emplace((*partition_spec).Copy());
	}
	if (partition_spec_tmp.has_value()) {
		builder.SetPartitionSpec(std::move(*partition_spec_tmp));
	}
	optional<SortOrder> write_order_tmp;
	if (write_order.has_value()) {
		write_order_tmp.emplace((*write_order).Copy());
	}
	if (write_order_tmp.has_value()) {
		builder.SetWriteOrder(std::move(*write_order_tmp));
	}
	optional<bool> stage_create_tmp;
	if (stage_create.has_value()) {
		stage_create_tmp.emplace();
		(*stage_create_tmp) = (*stage_create);
	}
	if (stage_create_tmp.has_value()) {
		builder.SetStageCreate(std::move((*stage_create_tmp)));
	}
	optional<case_insensitive_map_t<string>> properties_tmp;
	if (properties.has_value()) {
		properties_tmp.emplace();
		for (auto &entry : (*properties)) {
			(*properties_tmp).emplace(entry.first, entry.second);
		}
	}
	if (properties_tmp.has_value()) {
		builder.SetProperties(std::move((*properties_tmp)));
	}
	return builder.Build();
}

string CreateTableRequest::Validate() const {
	string error;
	error = schema.Validate();
	if (!error.empty()) {
		return error;
	}
	if (partition_spec.has_value()) {
		error = (*partition_spec).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (write_order.has_value()) {
		error = (*write_order).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void CreateTableRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());

	// Serialize: schema
	yyjson_mut_val *schema_val = schema.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "schema", schema_val);

	// Serialize: location
	if (location.has_value()) {
		auto &location_value = *location;
		yyjson_mut_obj_add_strcpy(doc, obj, "location", location_value.c_str());
	}

	// Serialize: partition-spec
	if (partition_spec.has_value()) {
		auto &partition_spec_value = *partition_spec;
		yyjson_mut_val *partition_spec_value_val = partition_spec_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "partition-spec", partition_spec_value_val);
	}

	// Serialize: write-order
	if (write_order.has_value()) {
		auto &write_order_value = *write_order;
		yyjson_mut_val *write_order_value_val = write_order_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "write-order", write_order_value_val);
	}

	// Serialize: stage-create
	if (stage_create.has_value()) {
		auto &stage_create_value = *stage_create;
		yyjson_mut_obj_add_bool(doc, obj, "stage-create", stage_create_value);
	}

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

yyjson_mut_val *CreateTableRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
