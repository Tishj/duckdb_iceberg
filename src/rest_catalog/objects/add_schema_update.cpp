
#include "rest_catalog/objects/add_schema_update.hpp"

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

AddSchemaUpdate::AddSchemaUpdate(BaseUpdate base_update_p, Schema schema_p, optional<int32_t> last_column_id_p)
    : base_update(std::move(base_update_p)), schema(std::move(schema_p)), last_column_id(std::move(last_column_id_p)) {
}
AddSchemaUpdate::AddSchemaUpdate(const AddSchemaUpdate &other)
    : base_update(other.base_update.Copy()), schema(other.schema.Copy()),
      last_column_id(
          (other.last_column_id.has_value() ? optional<int32_t>((*other.last_column_id)) : optional<int32_t>())) {
}
AddSchemaUpdate::AddSchemaUpdate(AddSchemaUpdate &&other)
    : AddSchemaUpdate(static_cast<const AddSchemaUpdate &>(other)) {
}

AddSchemaUpdateBuilder::AddSchemaUpdateBuilder() {
}

AddSchemaUpdateBuilder &AddSchemaUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AddSchemaUpdateBuilder &AddSchemaUpdateBuilder::SetSchema(Schema value) {
	schema_.emplace(std::move(value));
	has_schema_ = true;
	return *this;
}

AddSchemaUpdateBuilder &AddSchemaUpdateBuilder::SetLastColumnId(int32_t value) {
	last_column_id_.emplace(std::move(value));
	return *this;
}

AddSchemaUpdate AddSchemaUpdateBuilder::Build() {
	if (!has_schema_) {
		throw InvalidInputException("AddSchemaUpdate required property 'schema' is missing");
	}
	auto result = AddSchemaUpdate(std::move(*base_update_), std::move(*schema_), std::move(last_column_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> AddSchemaUpdateBuilder::TryBuild(optional<AddSchemaUpdate> &result) {
	if (!has_schema_) {
		return "AddSchemaUpdate required property 'schema' is missing";
	}
	auto built = AddSchemaUpdate(std::move(*base_update_), std::move(*schema_), std::move(last_column_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> AddSchemaUpdate::TryFromJSON(yyjson_val *obj, AddSchemaUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto schema_val = yyjson_obj_get(obj, "schema");
		if (!schema_val) {
			throw InvalidInputException("AddSchemaUpdate required property 'schema' is missing");
		} else {
			builder.SetSchema(Schema::FromJSON(schema_val));
		}
		auto last_column_id_val = yyjson_obj_get(obj, "last-column-id");
		if (last_column_id_val) {
			int32_t last_column_id;
			if (yyjson_is_int(last_column_id_val)) {
				last_column_id = yyjson_get_int(last_column_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "AddSchemaUpdate property 'last_column_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(last_column_id_val)));
			}
			builder.SetLastColumnId(std::move(last_column_id));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddSchemaUpdate AddSchemaUpdate::FromJSON(yyjson_val *obj) {
	AddSchemaUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

AddSchemaUpdate AddSchemaUpdate::Copy() const {
	return AddSchemaUpdate(*this);
}

optional<string> AddSchemaUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	error = schema.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void AddSchemaUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: schema
	yyjson_mut_val *schema_val = schema.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "schema", schema_val);

	// Serialize: last-column-id
	if (last_column_id.has_value()) {
		auto &last_column_id_value = *last_column_id;
		yyjson_mut_obj_add_int(doc, obj, "last-column-id", last_column_id_value);
	}
}

yyjson_mut_val *AddSchemaUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
