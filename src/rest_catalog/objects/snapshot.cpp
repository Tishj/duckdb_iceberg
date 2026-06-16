
#include "rest_catalog/objects/snapshot.hpp"

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

Snapshot::Snapshot(int64_t snapshot_id_p, int64_t timestamp_ms_p, string manifest_list_p, Object2 summary_p,
                   optional<int64_t> parent_snapshot_id_p, optional<int64_t> sequence_number_p,
                   optional<int64_t> first_row_id_p, optional<int64_t> added_rows_p, optional<int32_t> schema_id_p)
    : snapshot_id(std::move(snapshot_id_p)), timestamp_ms(std::move(timestamp_ms_p)),
      manifest_list(std::move(manifest_list_p)), summary(std::move(summary_p)),
      parent_snapshot_id(std::move(parent_snapshot_id_p)), sequence_number(std::move(sequence_number_p)),
      first_row_id(std::move(first_row_id_p)), added_rows(std::move(added_rows_p)), schema_id(std::move(schema_id_p)) {
}
Snapshot::Object2::Object2(string operation_p, case_insensitive_map_t<string> additional_properties_p)
    : operation(std::move(operation_p)), additional_properties(std::move(additional_properties_p)) {
}

Snapshot::Object2Builder::Object2Builder() {
}

Snapshot::Object2Builder &Snapshot::Object2Builder::SetOperation(string value) {
	operation_.emplace(std::move(value));
	has_operation_ = true;
	return *this;
}

Snapshot::Object2Builder &Snapshot::Object2Builder::SetAdditionalProperties(case_insensitive_map_t<string> value) {
	additional_properties_.emplace(std::move(value));
	return *this;
}

Snapshot::Object2 Snapshot::Object2Builder::Build() {
	if (!has_operation_) {
		throw InvalidInputException("Object2 required property 'operation' is missing");
	}
	auto result = Snapshot::Object2(std::move(*operation_), additional_properties_.has_value()
	                                                            ? std::move(*additional_properties_)
	                                                            : case_insensitive_map_t<string>());
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string Snapshot::Object2Builder::TryBuild(optional<Snapshot::Object2> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string Snapshot::Object2::TryFromJSON(yyjson_val *obj, Object2Builder &builder) {
	try {
		auto operation_val = yyjson_obj_get(obj, "operation");
		if (!operation_val) {
			throw InvalidInputException("Object2 required property 'operation' is missing");
		} else {
			string operation;
			if (yyjson_is_str(operation_val)) {
				operation = yyjson_get_str(operation_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Object2 property 'operation' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(operation_val)));
			}
			builder.SetOperation(std::move(operation));
		}
		case_insensitive_map_t<string> additional_properties;
		case_insensitive_set_t handled_properties {"operation"};
		size_t idx, max;
		yyjson_val *key, *val;
		yyjson_obj_foreach(obj, idx, max, key, val) {
			auto key_str = yyjson_get_str(key);
			if (handled_properties.count(key_str)) {
				continue;
			}
			string tmp;
			if (yyjson_is_str(val)) {
				tmp = yyjson_get_str(val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "Object2 property 'tmp' is not of type 'string', found '%s' instead", yyjson_get_type_desc(val)));
			}
			additional_properties.emplace(key_str, std::move(tmp));
		}
		builder.SetAdditionalProperties(std::move(additional_properties));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Snapshot::Object2 Snapshot::Object2::FromJSON(yyjson_val *obj) {
	Object2Builder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

Snapshot::Object2 Snapshot::Object2::Copy() const {
	Object2Builder builder;
	string operation_tmp;
	operation_tmp = operation;
	builder.SetOperation(std::move(operation_tmp));
	case_insensitive_map_t<string> additional_properties_tmp;
	for (auto &entry : additional_properties) {
		additional_properties_tmp.emplace(entry.first, entry.second);
	}
	builder.SetAdditionalProperties(std::move(additional_properties_tmp));
	return builder.Build();
}

string Snapshot::Object2::Validate() const {
	string error;
	if (!StringUtil::CIEquals(operation, "append") && !StringUtil::CIEquals(operation, "replace") &&
	    !StringUtil::CIEquals(operation, "overwrite") && !StringUtil::CIEquals(operation, "delete")) {
		return StringUtil::Format(
		    "Object2 property 'operation' must be one of [append, replace, overwrite, delete], not %s", operation);
	}
	return "";
}

void Snapshot::Object2::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: operation
	yyjson_mut_obj_add_strcpy(doc, obj, "operation", operation.c_str());

	// Serialize additional properties
	for (const auto &it : additional_properties) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, obj, key_ptr, value.c_str());
	}
}

yyjson_mut_val *Snapshot::Object2::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

SnapshotBuilder::SnapshotBuilder() {
}

SnapshotBuilder &SnapshotBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetTimestampMs(int64_t value) {
	timestamp_ms_.emplace(std::move(value));
	has_timestamp_ms_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetManifestList(string value) {
	manifest_list_.emplace(std::move(value));
	has_manifest_list_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSummary(Snapshot::Object2 value) {
	summary_.emplace(std::move(value));
	has_summary_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetParentSnapshotId(int64_t value) {
	parent_snapshot_id_.emplace(std::move(value));
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSequenceNumber(int64_t value) {
	sequence_number_.emplace(std::move(value));
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetFirstRowId(int64_t value) {
	first_row_id_.emplace(std::move(value));
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetAddedRows(int64_t value) {
	added_rows_.emplace(std::move(value));
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSchemaId(int32_t value) {
	schema_id_.emplace(std::move(value));
	return *this;
}

Snapshot SnapshotBuilder::Build() {
	if (!has_snapshot_id_) {
		throw InvalidInputException("Snapshot required property 'snapshot-id' is missing");
	}
	if (!has_timestamp_ms_) {
		throw InvalidInputException("Snapshot required property 'timestamp-ms' is missing");
	}
	if (!has_manifest_list_) {
		throw InvalidInputException("Snapshot required property 'manifest-list' is missing");
	}
	if (!has_summary_) {
		throw InvalidInputException("Snapshot required property 'summary' is missing");
	}
	auto result = Snapshot(std::move(*snapshot_id_), std::move(*timestamp_ms_), std::move(*manifest_list_),
	                       std::move(*summary_), std::move(parent_snapshot_id_), std::move(sequence_number_),
	                       std::move(first_row_id_), std::move(added_rows_), std::move(schema_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SnapshotBuilder::TryBuild(optional<Snapshot> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string Snapshot::TryFromJSON(yyjson_val *obj, SnapshotBuilder &builder) {
	try {
		auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
		if (!snapshot_id_val) {
			throw InvalidInputException("Snapshot required property 'snapshot-id' is missing");
		} else {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'snapshot_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
		auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
		if (!timestamp_ms_val) {
			throw InvalidInputException("Snapshot required property 'timestamp-ms' is missing");
		} else {
			int64_t timestamp_ms;
			if (yyjson_is_sint(timestamp_ms_val)) {
				timestamp_ms = yyjson_get_sint(timestamp_ms_val);
			} else if (yyjson_is_uint(timestamp_ms_val)) {
				timestamp_ms = yyjson_get_uint(timestamp_ms_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'timestamp_ms' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(timestamp_ms_val)));
			}
			builder.SetTimestampMs(std::move(timestamp_ms));
		}
		auto manifest_list_val = yyjson_obj_get(obj, "manifest-list");
		if (!manifest_list_val) {
			throw InvalidInputException("Snapshot required property 'manifest-list' is missing");
		} else {
			string manifest_list;
			if (yyjson_is_str(manifest_list_val)) {
				manifest_list = yyjson_get_str(manifest_list_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'manifest_list' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(manifest_list_val)));
			}
			builder.SetManifestList(std::move(manifest_list));
		}
		auto summary_val = yyjson_obj_get(obj, "summary");
		if (!summary_val) {
			throw InvalidInputException("Snapshot required property 'summary' is missing");
		} else {
			builder.SetSummary(Object2::FromJSON(summary_val));
		}
		auto parent_snapshot_id_val = yyjson_obj_get(obj, "parent-snapshot-id");
		if (parent_snapshot_id_val) {
			int64_t parent_snapshot_id;
			if (yyjson_is_sint(parent_snapshot_id_val)) {
				parent_snapshot_id = yyjson_get_sint(parent_snapshot_id_val);
			} else if (yyjson_is_uint(parent_snapshot_id_val)) {
				parent_snapshot_id = yyjson_get_uint(parent_snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "Snapshot property 'parent_snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(parent_snapshot_id_val)));
			}
			builder.SetParentSnapshotId(std::move(parent_snapshot_id));
		}
		auto sequence_number_val = yyjson_obj_get(obj, "sequence-number");
		if (sequence_number_val) {
			int64_t sequence_number;
			if (yyjson_is_sint(sequence_number_val)) {
				sequence_number = yyjson_get_sint(sequence_number_val);
			} else if (yyjson_is_uint(sequence_number_val)) {
				sequence_number = yyjson_get_uint(sequence_number_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "Snapshot property 'sequence_number' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(sequence_number_val)));
			}
			builder.SetSequenceNumber(std::move(sequence_number));
		}
		auto first_row_id_val = yyjson_obj_get(obj, "first-row-id");
		if (first_row_id_val) {
			int64_t first_row_id;
			if (yyjson_is_sint(first_row_id_val)) {
				first_row_id = yyjson_get_sint(first_row_id_val);
			} else if (yyjson_is_uint(first_row_id_val)) {
				first_row_id = yyjson_get_uint(first_row_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'first_row_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(first_row_id_val)));
			}
			builder.SetFirstRowId(std::move(first_row_id));
		}
		auto added_rows_val = yyjson_obj_get(obj, "added-rows");
		if (added_rows_val) {
			int64_t added_rows;
			if (yyjson_is_sint(added_rows_val)) {
				added_rows = yyjson_get_sint(added_rows_val);
			} else if (yyjson_is_uint(added_rows_val)) {
				added_rows = yyjson_get_uint(added_rows_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'added_rows' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(added_rows_val)));
			}
			builder.SetAddedRows(std::move(added_rows));
		}
		auto schema_id_val = yyjson_obj_get(obj, "schema-id");
		if (schema_id_val) {
			int32_t schema_id;
			if (yyjson_is_int(schema_id_val)) {
				schema_id = yyjson_get_int(schema_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("Snapshot property 'schema_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(schema_id_val)));
			}
			builder.SetSchemaId(std::move(schema_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Snapshot Snapshot::FromJSON(yyjson_val *obj) {
	SnapshotBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

Snapshot Snapshot::Copy() const {
	SnapshotBuilder builder;
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	int64_t timestamp_ms_tmp;
	timestamp_ms_tmp = timestamp_ms;
	builder.SetTimestampMs(std::move(timestamp_ms_tmp));
	string manifest_list_tmp;
	manifest_list_tmp = manifest_list;
	builder.SetManifestList(std::move(manifest_list_tmp));
	auto summary_tmp = summary.Copy();
	builder.SetSummary(std::move(summary_tmp));
	optional<int64_t> parent_snapshot_id_tmp;
	if (parent_snapshot_id.has_value()) {
		parent_snapshot_id_tmp.emplace();
		(*parent_snapshot_id_tmp) = (*parent_snapshot_id);
	}
	if (parent_snapshot_id_tmp.has_value()) {
		builder.SetParentSnapshotId(std::move((*parent_snapshot_id_tmp)));
	}
	optional<int64_t> sequence_number_tmp;
	if (sequence_number.has_value()) {
		sequence_number_tmp.emplace();
		(*sequence_number_tmp) = (*sequence_number);
	}
	if (sequence_number_tmp.has_value()) {
		builder.SetSequenceNumber(std::move((*sequence_number_tmp)));
	}
	optional<int64_t> first_row_id_tmp;
	if (first_row_id.has_value()) {
		first_row_id_tmp.emplace();
		(*first_row_id_tmp) = (*first_row_id);
	}
	if (first_row_id_tmp.has_value()) {
		builder.SetFirstRowId(std::move((*first_row_id_tmp)));
	}
	optional<int64_t> added_rows_tmp;
	if (added_rows.has_value()) {
		added_rows_tmp.emplace();
		(*added_rows_tmp) = (*added_rows);
	}
	if (added_rows_tmp.has_value()) {
		builder.SetAddedRows(std::move((*added_rows_tmp)));
	}
	optional<int32_t> schema_id_tmp;
	if (schema_id.has_value()) {
		schema_id_tmp.emplace();
		(*schema_id_tmp) = (*schema_id);
	}
	if (schema_id_tmp.has_value()) {
		builder.SetSchemaId(std::move((*schema_id_tmp)));
	}
	return builder.Build();
}

string Snapshot::Validate() const {
	string error;
	error = summary.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void Snapshot::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: timestamp-ms
	yyjson_mut_obj_add_sint(doc, obj, "timestamp-ms", timestamp_ms);

	// Serialize: manifest-list
	yyjson_mut_obj_add_strcpy(doc, obj, "manifest-list", manifest_list.c_str());

	// Serialize: summary
	yyjson_mut_val *summary_val = summary.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "summary", summary_val);

	// Serialize: parent-snapshot-id
	if (parent_snapshot_id.has_value()) {
		auto &parent_snapshot_id_value = *parent_snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "parent-snapshot-id", parent_snapshot_id_value);
	}

	// Serialize: sequence-number
	if (sequence_number.has_value()) {
		auto &sequence_number_value = *sequence_number;
		yyjson_mut_obj_add_sint(doc, obj, "sequence-number", sequence_number_value);
	}

	// Serialize: first-row-id
	if (first_row_id.has_value()) {
		auto &first_row_id_value = *first_row_id;
		yyjson_mut_obj_add_sint(doc, obj, "first-row-id", first_row_id_value);
	}

	// Serialize: added-rows
	if (added_rows.has_value()) {
		auto &added_rows_value = *added_rows;
		yyjson_mut_obj_add_sint(doc, obj, "added-rows", added_rows_value);
	}

	// Serialize: schema-id
	if (schema_id.has_value()) {
		auto &schema_id_value = *schema_id;
		yyjson_mut_obj_add_int(doc, obj, "schema-id", schema_id_value);
	}
}

yyjson_mut_val *Snapshot::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
