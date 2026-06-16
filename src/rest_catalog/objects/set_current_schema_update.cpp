
#include "rest_catalog/objects/set_current_schema_update.hpp"

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

SetCurrentSchemaUpdate::SetCurrentSchemaUpdate(BaseUpdate base_update_p, int32_t schema_id_p)
    : base_update(std::move(base_update_p)), schema_id(std::move(schema_id_p)) {
}
SetCurrentSchemaUpdate::SetCurrentSchemaUpdate(const SetCurrentSchemaUpdate &other)
    : base_update(other.base_update.Copy()), schema_id(other.schema_id) {
}
SetCurrentSchemaUpdate::SetCurrentSchemaUpdate(SetCurrentSchemaUpdate &&other)
    : SetCurrentSchemaUpdate(static_cast<const SetCurrentSchemaUpdate &>(other)) {
}

SetCurrentSchemaUpdateBuilder::SetCurrentSchemaUpdateBuilder() {
}

SetCurrentSchemaUpdateBuilder &SetCurrentSchemaUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetCurrentSchemaUpdateBuilder &SetCurrentSchemaUpdateBuilder::SetSchemaId(int32_t value) {
	schema_id_.emplace(std::move(value));
	has_schema_id_ = true;
	return *this;
}

SetCurrentSchemaUpdate SetCurrentSchemaUpdateBuilder::Build() {
	if (!has_schema_id_) {
		throw InvalidInputException("SetCurrentSchemaUpdate required property 'schema-id' is missing");
	}
	auto result = SetCurrentSchemaUpdate(std::move(*base_update_), std::move(*schema_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> SetCurrentSchemaUpdateBuilder::TryBuild(optional<SetCurrentSchemaUpdate> &result) {
	if (!has_schema_id_) {
		return "SetCurrentSchemaUpdate required property 'schema-id' is missing";
	}
	auto built = SetCurrentSchemaUpdate(std::move(*base_update_), std::move(*schema_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> SetCurrentSchemaUpdate::TryFromJSON(yyjson_val *obj, SetCurrentSchemaUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto schema_id_val = yyjson_obj_get(obj, "schema-id");
		if (!schema_id_val) {
			throw InvalidInputException("SetCurrentSchemaUpdate required property 'schema-id' is missing");
		} else {
			int32_t schema_id;
			if (yyjson_is_int(schema_id_val)) {
				schema_id = yyjson_get_int(schema_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "SetCurrentSchemaUpdate property 'schema_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(schema_id_val)));
			}
			builder.SetSchemaId(std::move(schema_id));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetCurrentSchemaUpdate SetCurrentSchemaUpdate::FromJSON(yyjson_val *obj) {
	SetCurrentSchemaUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

SetCurrentSchemaUpdate SetCurrentSchemaUpdate::Copy() const {
	return SetCurrentSchemaUpdate(*this);
}

optional<string> SetCurrentSchemaUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void SetCurrentSchemaUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: schema-id
	yyjson_mut_obj_add_int(doc, obj, "schema-id", schema_id);
}

yyjson_mut_val *SetCurrentSchemaUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
