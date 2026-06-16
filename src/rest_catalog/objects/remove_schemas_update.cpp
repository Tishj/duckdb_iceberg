
#include "rest_catalog/objects/remove_schemas_update.hpp"

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

RemoveSchemasUpdate::RemoveSchemasUpdate(BaseUpdate base_update_p, vector<int32_t> schema_ids_p)
    : base_update(std::move(base_update_p)), schema_ids(std::move(schema_ids_p)) {
}
RemoveSchemasUpdate::RemoveSchemasUpdate(const RemoveSchemasUpdate &other)
    : base_update(other.base_update.Copy()), schema_ids(([&]() {
	      vector<int32_t> copied;
	      copied.reserve(other.schema_ids.size());
	      for (const auto &item : other.schema_ids) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())) {
}
RemoveSchemasUpdate::RemoveSchemasUpdate(RemoveSchemasUpdate &&other)
    : RemoveSchemasUpdate(static_cast<const RemoveSchemasUpdate &>(other)) {
}

RemoveSchemasUpdateBuilder::RemoveSchemasUpdateBuilder() {
}

RemoveSchemasUpdateBuilder &RemoveSchemasUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

RemoveSchemasUpdateBuilder &RemoveSchemasUpdateBuilder::SetSchemaIds(vector<int32_t> value) {
	schema_ids_.emplace(std::move(value));
	has_schema_ids_ = true;
	return *this;
}

RemoveSchemasUpdate RemoveSchemasUpdateBuilder::Build() {
	if (!has_schema_ids_) {
		throw InvalidInputException("RemoveSchemasUpdate required property 'schema-ids' is missing");
	}
	auto result = RemoveSchemasUpdate(std::move(*base_update_), std::move(*schema_ids_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemoveSchemasUpdateBuilder::TryBuild(optional<RemoveSchemasUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string RemoveSchemasUpdate::TryFromJSON(yyjson_val *obj, RemoveSchemasUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto schema_ids_val = yyjson_obj_get(obj, "schema-ids");
		if (!schema_ids_val) {
			throw InvalidInputException("RemoveSchemasUpdate required property 'schema-ids' is missing");
		} else {
			vector<int32_t> schema_ids;
			if (yyjson_is_arr(schema_ids_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(schema_ids_val, idx, max, val) {
					int32_t tmp;
					if (yyjson_is_int(val)) {
						tmp = yyjson_get_int(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "RemoveSchemasUpdate property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					schema_ids.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RemoveSchemasUpdate property 'schema_ids' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(schema_ids_val)));
			}
			builder.SetSchemaIds(std::move(schema_ids));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveSchemasUpdate RemoveSchemasUpdate::FromJSON(yyjson_val *obj) {
	RemoveSchemasUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

RemoveSchemasUpdate RemoveSchemasUpdate::Copy() const {
	return RemoveSchemasUpdate(*this);
}

string RemoveSchemasUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void RemoveSchemasUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: schema-ids
	yyjson_mut_val *schema_ids_arr = yyjson_mut_arr(doc);
	for (const auto &item : schema_ids) {
		yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
		yyjson_mut_arr_append(schema_ids_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "schema-ids", schema_ids_arr);
}

yyjson_mut_val *RemoveSchemasUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
