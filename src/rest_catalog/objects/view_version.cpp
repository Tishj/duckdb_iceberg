
#include "rest_catalog/objects/view_version.hpp"

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

ViewVersion::ViewVersion(int32_t version_id_p, int64_t timestamp_ms_p, int32_t schema_id_p,
                         case_insensitive_map_t<string> summary_p, vector<ViewRepresentation> representations_p,
                         Namespace default_namespace_p, optional<string> default_catalog_p)
    : version_id(std::move(version_id_p)), timestamp_ms(std::move(timestamp_ms_p)), schema_id(std::move(schema_id_p)),
      summary(std::move(summary_p)), representations(std::move(representations_p)),
      default_namespace(std::move(default_namespace_p)), default_catalog(std::move(default_catalog_p)) {
}

ViewVersionBuilder::ViewVersionBuilder() {
}

ViewVersionBuilder &ViewVersionBuilder::SetVersionId(int32_t value) {
	version_id_.emplace(std::move(value));
	has_version_id_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetTimestampMs(int64_t value) {
	timestamp_ms_.emplace(std::move(value));
	has_timestamp_ms_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetSchemaId(int32_t value) {
	schema_id_.emplace(std::move(value));
	has_schema_id_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetSummary(case_insensitive_map_t<string> value) {
	summary_.emplace(std::move(value));
	has_summary_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetRepresentations(vector<ViewRepresentation> value) {
	representations_.emplace(std::move(value));
	has_representations_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetDefaultNamespace(Namespace value) {
	default_namespace_.emplace(std::move(value));
	has_default_namespace_ = true;
	return *this;
}

ViewVersionBuilder &ViewVersionBuilder::SetDefaultCatalog(string value) {
	default_catalog_.emplace(std::move(value));
	return *this;
}

ViewVersion ViewVersionBuilder::Build() {
	if (!has_version_id_) {
		throw InvalidInputException("ViewVersion required property 'version-id' is missing");
	}
	if (!has_timestamp_ms_) {
		throw InvalidInputException("ViewVersion required property 'timestamp-ms' is missing");
	}
	if (!has_schema_id_) {
		throw InvalidInputException("ViewVersion required property 'schema-id' is missing");
	}
	if (!has_summary_) {
		throw InvalidInputException("ViewVersion required property 'summary' is missing");
	}
	if (!has_representations_) {
		throw InvalidInputException("ViewVersion required property 'representations' is missing");
	}
	if (!has_default_namespace_) {
		throw InvalidInputException("ViewVersion required property 'default-namespace' is missing");
	}
	auto result =
	    ViewVersion(std::move(*version_id_), std::move(*timestamp_ms_), std::move(*schema_id_), std::move(*summary_),
	                std::move(*representations_), std::move(*default_namespace_), std::move(default_catalog_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewVersionBuilder::TryBuild(optional<ViewVersion> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewVersion ViewVersion::FromJSON(yyjson_val *obj) {
	ViewVersionBuilder builder;
	auto version_id_val = yyjson_obj_get(obj, "version-id");
	if (!version_id_val) {
		throw InvalidInputException("ViewVersion required property 'version-id' is missing");
	} else {
		int32_t version_id;
		if (yyjson_is_int(version_id_val)) {
			version_id = yyjson_get_int(version_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ViewVersion property 'version_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(version_id_val)));
		}
		builder.SetVersionId(std::move(version_id));
	}
	auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
	if (!timestamp_ms_val) {
		throw InvalidInputException("ViewVersion required property 'timestamp-ms' is missing");
	} else {
		int64_t timestamp_ms;
		if (yyjson_is_sint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_sint(timestamp_ms_val);
		} else if (yyjson_is_uint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_uint(timestamp_ms_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ViewVersion property 'timestamp_ms' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(timestamp_ms_val)));
		}
		builder.SetTimestampMs(std::move(timestamp_ms));
	}
	auto schema_id_val = yyjson_obj_get(obj, "schema-id");
	if (!schema_id_val) {
		throw InvalidInputException("ViewVersion required property 'schema-id' is missing");
	} else {
		int32_t schema_id;
		if (yyjson_is_int(schema_id_val)) {
			schema_id = yyjson_get_int(schema_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ViewVersion property 'schema_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(schema_id_val)));
		}
		builder.SetSchemaId(std::move(schema_id));
	}
	auto summary_val = yyjson_obj_get(obj, "summary");
	if (!summary_val) {
		throw InvalidInputException("ViewVersion required property 'summary' is missing");
	} else {
		case_insensitive_map_t<string> summary;
		if (yyjson_is_obj(summary_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(summary_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ViewVersion property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				summary.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("ViewVersion property 'summary' is not of type 'object'");
		}
		builder.SetSummary(std::move(summary));
	}
	auto representations_val = yyjson_obj_get(obj, "representations");
	if (!representations_val) {
		throw InvalidInputException("ViewVersion required property 'representations' is missing");
	} else {
		vector<ViewRepresentation> representations;
		if (yyjson_is_arr(representations_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(representations_val, idx, max, val) {
				auto tmp = ViewRepresentation::FromJSON(val);
				representations.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ViewVersion property 'representations' is not of type 'array', found '%s' instead",
			                       yyjson_get_type_desc(representations_val)));
		}
		builder.SetRepresentations(std::move(representations));
	}
	auto default_namespace_val = yyjson_obj_get(obj, "default-namespace");
	if (!default_namespace_val) {
		throw InvalidInputException("ViewVersion required property 'default-namespace' is missing");
	} else {
		builder.SetDefaultNamespace(Namespace::FromJSON(default_namespace_val));
	}
	auto default_catalog_val = yyjson_obj_get(obj, "default-catalog");
	if (default_catalog_val) {
		string default_catalog;
		if (yyjson_is_str(default_catalog_val)) {
			default_catalog = yyjson_get_str(default_catalog_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ViewVersion property 'default_catalog' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(default_catalog_val)));
		}
		builder.SetDefaultCatalog(std::move(default_catalog));
	}
	return builder.Build();
}

string ViewVersion::TryFromJSON(yyjson_val *obj, optional<ViewVersion> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewVersion ViewVersion::Copy() const {
	ViewVersionBuilder builder;
	int32_t version_id_tmp;
	version_id_tmp = version_id;
	builder.SetVersionId(std::move(version_id_tmp));
	int64_t timestamp_ms_tmp;
	timestamp_ms_tmp = timestamp_ms;
	builder.SetTimestampMs(std::move(timestamp_ms_tmp));
	int32_t schema_id_tmp;
	schema_id_tmp = schema_id;
	builder.SetSchemaId(std::move(schema_id_tmp));
	case_insensitive_map_t<string> summary_tmp;
	for (auto &entry : summary) {
		summary_tmp.emplace(entry.first, entry.second);
	}
	builder.SetSummary(std::move(summary_tmp));
	vector<ViewRepresentation> representations_tmp;
	representations_tmp.reserve(representations.size());
	for (auto &item : representations) {
		representations_tmp.emplace_back(item.Copy());
	}
	builder.SetRepresentations(std::move(representations_tmp));
	auto default_namespace_tmp = default_namespace.Copy();
	builder.SetDefaultNamespace(std::move(default_namespace_tmp));
	optional<string> default_catalog_tmp;
	if (default_catalog.has_value()) {
		default_catalog_tmp.emplace();
		(*default_catalog_tmp) = (*default_catalog);
	}
	if (default_catalog_tmp.has_value()) {
		builder.SetDefaultCatalog(std::move((*default_catalog_tmp)));
	}
	return builder.Build();
}

string ViewVersion::Validate() const {
	string error;
	for (const auto &item : representations) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	error = default_namespace.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void ViewVersion::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: version-id
	yyjson_mut_obj_add_int(doc, obj, "version-id", version_id);

	// Serialize: timestamp-ms
	yyjson_mut_obj_add_sint(doc, obj, "timestamp-ms", timestamp_ms);

	// Serialize: schema-id
	yyjson_mut_obj_add_int(doc, obj, "schema-id", schema_id);

	// Serialize: summary
	yyjson_mut_val *summary_obj = yyjson_mut_obj(doc);
	for (const auto &it : summary) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, summary_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "summary", summary_obj);

	// Serialize: representations
	yyjson_mut_val *representations_arr = yyjson_mut_arr(doc);
	for (const auto &item : representations) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(representations_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "representations", representations_arr);

	// Serialize: default-namespace
	yyjson_mut_val *default_namespace_val = default_namespace.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "default-namespace", default_namespace_val);

	// Serialize: default-catalog
	if (default_catalog.has_value()) {
		auto &default_catalog_value = *default_catalog;
		yyjson_mut_obj_add_strcpy(doc, obj, "default-catalog", default_catalog_value.c_str());
	}
}

yyjson_mut_val *ViewVersion::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
