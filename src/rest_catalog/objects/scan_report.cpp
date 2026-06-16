
#include "rest_catalog/objects/scan_report.hpp"

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

ScanReport::ScanReport(string table_name_p, int64_t snapshot_id_p, unique_ptr<Expression> filter_p, int32_t schema_id_p,
                       vector<int32_t> projected_field_ids_p, vector<string> projected_field_names_p, Metrics metrics_p,
                       optional<case_insensitive_map_t<string>> metadata_p)
    : table_name(std::move(table_name_p)), snapshot_id(std::move(snapshot_id_p)), filter(std::move(filter_p)),
      schema_id(std::move(schema_id_p)), projected_field_ids(std::move(projected_field_ids_p)),
      projected_field_names(std::move(projected_field_names_p)), metrics(std::move(metrics_p)),
      metadata(std::move(metadata_p)) {
}

ScanReportBuilder::ScanReportBuilder() {
}

ScanReportBuilder &ScanReportBuilder::SetTableName(string value) {
	table_name_.emplace(std::move(value));
	has_table_name_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetFilter(unique_ptr<Expression> value) {
	filter_ = std::move(value);
	has_filter_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetSchemaId(int32_t value) {
	schema_id_.emplace(std::move(value));
	has_schema_id_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetProjectedFieldIds(vector<int32_t> value) {
	projected_field_ids_.emplace(std::move(value));
	has_projected_field_ids_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetProjectedFieldNames(vector<string> value) {
	projected_field_names_.emplace(std::move(value));
	has_projected_field_names_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetMetrics(Metrics value) {
	metrics_.emplace(std::move(value));
	has_metrics_ = true;
	return *this;
}

ScanReportBuilder &ScanReportBuilder::SetMetadata(case_insensitive_map_t<string> value) {
	metadata_.emplace(std::move(value));
	return *this;
}

ScanReport ScanReportBuilder::Build() {
	if (!has_table_name_) {
		throw InvalidInputException("ScanReport required property 'table-name' is missing");
	}
	if (!has_snapshot_id_) {
		throw InvalidInputException("ScanReport required property 'snapshot-id' is missing");
	}
	if (!has_filter_) {
		throw InvalidInputException("ScanReport required property 'filter' is missing");
	}
	if (!has_schema_id_) {
		throw InvalidInputException("ScanReport required property 'schema-id' is missing");
	}
	if (!has_projected_field_ids_) {
		throw InvalidInputException("ScanReport required property 'projected-field-ids' is missing");
	}
	if (!has_projected_field_names_) {
		throw InvalidInputException("ScanReport required property 'projected-field-names' is missing");
	}
	if (!has_metrics_) {
		throw InvalidInputException("ScanReport required property 'metrics' is missing");
	}
	auto result = ScanReport(std::move(*table_name_), std::move(*snapshot_id_), std::move(filter_),
	                         std::move(*schema_id_), std::move(*projected_field_ids_),
	                         std::move(*projected_field_names_), std::move(*metrics_), std::move(metadata_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ScanReportBuilder::TryBuild(optional<ScanReport> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ScanReport ScanReport::FromJSON(yyjson_val *obj) {
	ScanReportBuilder builder;
	auto table_name_val = yyjson_obj_get(obj, "table-name");
	if (!table_name_val) {
		throw InvalidInputException("ScanReport required property 'table-name' is missing");
	} else {
		string table_name;
		if (yyjson_is_str(table_name_val)) {
			table_name = yyjson_get_str(table_name_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ScanReport property 'table_name' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(table_name_val)));
		}
		builder.SetTableName(std::move(table_name));
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		throw InvalidInputException("ScanReport required property 'snapshot-id' is missing");
	} else {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ScanReport property 'snapshot_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	auto filter_val = yyjson_obj_get(obj, "filter");
	if (!filter_val) {
		throw InvalidInputException("ScanReport required property 'filter' is missing");
	} else {
		unique_ptr<Expression> filter;
		filter = make_uniq<Expression>(Expression::FromJSON(filter_val));
		builder.SetFilter(std::move(filter));
	}
	auto schema_id_val = yyjson_obj_get(obj, "schema-id");
	if (!schema_id_val) {
		throw InvalidInputException("ScanReport required property 'schema-id' is missing");
	} else {
		int32_t schema_id;
		if (yyjson_is_int(schema_id_val)) {
			schema_id = yyjson_get_int(schema_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ScanReport property 'schema_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(schema_id_val)));
		}
		builder.SetSchemaId(std::move(schema_id));
	}
	auto projected_field_ids_val = yyjson_obj_get(obj, "projected-field-ids");
	if (!projected_field_ids_val) {
		throw InvalidInputException("ScanReport required property 'projected-field-ids' is missing");
	} else {
		vector<int32_t> projected_field_ids;
		if (yyjson_is_arr(projected_field_ids_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(projected_field_ids_val, idx, max, val) {
				int32_t tmp;
				if (yyjson_is_int(val)) {
					tmp = yyjson_get_int(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ScanReport property 'tmp' is not of type 'integer', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				projected_field_ids.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "ScanReport property 'projected_field_ids' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(projected_field_ids_val)));
		}
		builder.SetProjectedFieldIds(std::move(projected_field_ids));
	}
	auto projected_field_names_val = yyjson_obj_get(obj, "projected-field-names");
	if (!projected_field_names_val) {
		throw InvalidInputException("ScanReport required property 'projected-field-names' is missing");
	} else {
		vector<string> projected_field_names;
		if (yyjson_is_arr(projected_field_names_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(projected_field_names_val, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ScanReport property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				projected_field_names.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "ScanReport property 'projected_field_names' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(projected_field_names_val)));
		}
		builder.SetProjectedFieldNames(std::move(projected_field_names));
	}
	auto metrics_val = yyjson_obj_get(obj, "metrics");
	if (!metrics_val) {
		throw InvalidInputException("ScanReport required property 'metrics' is missing");
	} else {
		builder.SetMetrics(Metrics::FromJSON(metrics_val));
	}
	auto metadata_val = yyjson_obj_get(obj, "metadata");
	if (metadata_val) {
		case_insensitive_map_t<string> metadata;
		if (yyjson_is_obj(metadata_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(metadata_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ScanReport property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				metadata.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("ScanReport property 'metadata' is not of type 'object'");
		}
		builder.SetMetadata(std::move(metadata));
	}
	return builder.Build();
}

string ScanReport::TryFromJSON(yyjson_val *obj, optional<ScanReport> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ScanReport ScanReport::Copy() const {
	ScanReportBuilder builder;
	string table_name_tmp;
	table_name_tmp = table_name;
	builder.SetTableName(std::move(table_name_tmp));
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	unique_ptr<Expression> filter_tmp;
	filter_tmp = filter ? make_uniq<Expression>(filter->Copy()) : nullptr;
	builder.SetFilter(std::move(filter_tmp));
	int32_t schema_id_tmp;
	schema_id_tmp = schema_id;
	builder.SetSchemaId(std::move(schema_id_tmp));
	vector<int32_t> projected_field_ids_tmp;
	projected_field_ids_tmp.reserve(projected_field_ids.size());
	for (auto &item : projected_field_ids) {
		projected_field_ids_tmp.emplace_back(item);
	}
	builder.SetProjectedFieldIds(std::move(projected_field_ids_tmp));
	vector<string> projected_field_names_tmp;
	projected_field_names_tmp.reserve(projected_field_names.size());
	for (auto &item : projected_field_names) {
		projected_field_names_tmp.emplace_back(item);
	}
	builder.SetProjectedFieldNames(std::move(projected_field_names_tmp));
	auto metrics_tmp = metrics.Copy();
	builder.SetMetrics(std::move(metrics_tmp));
	optional<case_insensitive_map_t<string>> metadata_tmp;
	if (metadata.has_value()) {
		metadata_tmp.emplace();
		for (auto &entry : (*metadata)) {
			(*metadata_tmp).emplace(entry.first, entry.second);
		}
	}
	if (metadata_tmp.has_value()) {
		builder.SetMetadata(std::move((*metadata_tmp)));
	}
	return builder.Build();
}

string ScanReport::Validate() const {
	string error;
	error = filter->Validate();
	if (!error.empty()) {
		return error;
	}
	error = metrics.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void ScanReport::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: table-name
	yyjson_mut_obj_add_strcpy(doc, obj, "table-name", table_name.c_str());

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: filter
	yyjson_mut_val *filter_val = filter->ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "filter", filter_val);

	// Serialize: schema-id
	yyjson_mut_obj_add_int(doc, obj, "schema-id", schema_id);

	// Serialize: projected-field-ids
	yyjson_mut_val *projected_field_ids_arr = yyjson_mut_arr(doc);
	for (const auto &item : projected_field_ids) {
		yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
		yyjson_mut_arr_append(projected_field_ids_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "projected-field-ids", projected_field_ids_arr);

	// Serialize: projected-field-names
	yyjson_mut_val *projected_field_names_arr = yyjson_mut_arr(doc);
	for (const auto &item : projected_field_names) {
		yyjson_mut_val *item_val = yyjson_mut_str(doc, item.c_str());
		yyjson_mut_arr_append(projected_field_names_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "projected-field-names", projected_field_names_arr);

	// Serialize: metrics
	yyjson_mut_val *metrics_val = metrics.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "metrics", metrics_val);

	// Serialize: metadata
	if (metadata.has_value()) {
		auto &metadata_value = *metadata;
		yyjson_mut_val *metadata_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : metadata_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, metadata_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "metadata", metadata_value_obj);
	}
}

yyjson_mut_val *ScanReport::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
