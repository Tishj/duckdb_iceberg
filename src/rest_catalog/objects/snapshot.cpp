
#include "rest_catalog/objects/snapshot.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Snapshot::Snapshot() {
}
Snapshot::Object2::Object2() {
}

Snapshot::Object2Builder::Object2Builder() {
}

Snapshot::Object2Builder &Snapshot::Object2Builder::SetOperation(string value) {
	result_.operation = std::move(value);
	has_operation_ = true;
	return *this;
}

Snapshot::Object2Builder &Snapshot::Object2Builder::SetAdditionalProperties(case_insensitive_map_t<string> value) {
	result_.additional_properties = std::move(value);
	return *this;
}

string Snapshot::Object2Builder::TryBuild(Snapshot::Object2 &result) {
	if (!has_operation_) {
		return "Object2 required property 'operation' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Snapshot::Object2 Snapshot::Object2Builder::Build() {
	Snapshot::Object2 result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Snapshot::Object2 Snapshot::Object2::FromJSON(yyjson_val *obj) {
	Object2 res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Snapshot::Object2 Snapshot::Object2::Copy() const {
	Object2 res;
	res.operation = operation;
	for (auto &entry : additional_properties) {
		res.additional_properties.emplace(entry.first, entry.second);
	}
	return res;
}

string Snapshot::Object2::Validate() const {
	string error;
	if (operation != "append" && operation != "replace" && operation != "overwrite" && operation != "delete") {
		return "Object2 property 'operation' must be one of [append, replace, overwrite, delete]";
	}
	return "";
}

string Snapshot::Object2::TryFromJSON(yyjson_val *obj) {
	string error;
	auto operation_val = yyjson_obj_get(obj, "operation");
	if (!operation_val) {
		return "Object2 required property 'operation' is missing";
	} else {
		if (yyjson_is_str(operation_val)) {
			operation = yyjson_get_str(operation_val);
		} else {
			return StringUtil::Format("Object2 property 'operation' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(operation_val));
		}
	}
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
			return StringUtil::Format("Object2 property 'tmp' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(val));
		}
		additional_properties.emplace(key_str, std::move(tmp));
	}
	return Validate();
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
	result_.snapshot_id = std::move(value);
	has_snapshot_id_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetTimestampMs(int64_t value) {
	result_.timestamp_ms = std::move(value);
	has_timestamp_ms_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetManifestList(string value) {
	result_.manifest_list = std::move(value);
	has_manifest_list_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSummary(Snapshot::Object2 value) {
	result_.summary = std::move(value);
	has_summary_ = true;
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetParentSnapshotId(int64_t value) {
	result_.parent_snapshot_id = std::move(value);
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSequenceNumber(int64_t value) {
	result_.sequence_number = std::move(value);
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetFirstRowId(int64_t value) {
	result_.first_row_id = std::move(value);
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetAddedRows(int64_t value) {
	result_.added_rows = std::move(value);
	return *this;
}

SnapshotBuilder &SnapshotBuilder::SetSchemaId(int32_t value) {
	result_.schema_id = std::move(value);
	return *this;
}

string SnapshotBuilder::TryBuild(Snapshot &result) {
	if (!has_snapshot_id_) {
		return "Snapshot required property 'snapshot-id' is missing";
	}
	if (!has_timestamp_ms_) {
		return "Snapshot required property 'timestamp-ms' is missing";
	}
	if (!has_manifest_list_) {
		return "Snapshot required property 'manifest-list' is missing";
	}
	if (!has_summary_) {
		return "Snapshot required property 'summary' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Snapshot SnapshotBuilder::Build() {
	Snapshot result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Snapshot Snapshot::FromJSON(yyjson_val *obj) {
	Snapshot res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Snapshot Snapshot::Copy() const {
	Snapshot res;
	res.snapshot_id = snapshot_id;
	res.timestamp_ms = timestamp_ms;
	res.manifest_list = manifest_list;
	res.summary = summary.Copy();
	if (parent_snapshot_id.has_value()) {
		res.parent_snapshot_id.emplace();
		(*res.parent_snapshot_id) = (*parent_snapshot_id);
	}
	if (sequence_number.has_value()) {
		res.sequence_number.emplace();
		(*res.sequence_number) = (*sequence_number);
	}
	if (first_row_id.has_value()) {
		res.first_row_id.emplace();
		(*res.first_row_id) = (*first_row_id);
	}
	if (added_rows.has_value()) {
		res.added_rows.emplace();
		(*res.added_rows) = (*added_rows);
	}
	if (schema_id.has_value()) {
		res.schema_id.emplace();
		(*res.schema_id) = (*schema_id);
	}
	return res;
}

string Snapshot::Validate() const {
	string error;
	error = summary.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string Snapshot::TryFromJSON(yyjson_val *obj) {
	string error;
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		return "Snapshot required property 'snapshot-id' is missing";
	} else {
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			return StringUtil::Format("Snapshot property 'snapshot_id' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(snapshot_id_val));
		}
	}
	auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
	if (!timestamp_ms_val) {
		return "Snapshot required property 'timestamp-ms' is missing";
	} else {
		if (yyjson_is_sint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_sint(timestamp_ms_val);
		} else if (yyjson_is_uint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_uint(timestamp_ms_val);
		} else {
			return StringUtil::Format("Snapshot property 'timestamp_ms' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(timestamp_ms_val));
		}
	}
	auto manifest_list_val = yyjson_obj_get(obj, "manifest-list");
	if (!manifest_list_val) {
		return "Snapshot required property 'manifest-list' is missing";
	} else {
		if (yyjson_is_str(manifest_list_val)) {
			manifest_list = yyjson_get_str(manifest_list_val);
		} else {
			return StringUtil::Format("Snapshot property 'manifest_list' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(manifest_list_val));
		}
	}
	auto summary_val = yyjson_obj_get(obj, "summary");
	if (!summary_val) {
		return "Snapshot required property 'summary' is missing";
	} else {
		error = summary.TryFromJSON(summary_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto parent_snapshot_id_val = yyjson_obj_get(obj, "parent-snapshot-id");
	if (parent_snapshot_id_val) {
		int64_t parent_snapshot_id_tmp;
		if (yyjson_is_sint(parent_snapshot_id_val)) {
			parent_snapshot_id_tmp = yyjson_get_sint(parent_snapshot_id_val);
		} else if (yyjson_is_uint(parent_snapshot_id_val)) {
			parent_snapshot_id_tmp = yyjson_get_uint(parent_snapshot_id_val);
		} else {
			return StringUtil::Format(
			    "Snapshot property 'parent_snapshot_id_tmp' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(parent_snapshot_id_val));
		}
		parent_snapshot_id = std::move(parent_snapshot_id_tmp);
	}
	auto sequence_number_val = yyjson_obj_get(obj, "sequence-number");
	if (sequence_number_val) {
		int64_t sequence_number_tmp;
		if (yyjson_is_sint(sequence_number_val)) {
			sequence_number_tmp = yyjson_get_sint(sequence_number_val);
		} else if (yyjson_is_uint(sequence_number_val)) {
			sequence_number_tmp = yyjson_get_uint(sequence_number_val);
		} else {
			return StringUtil::Format(
			    "Snapshot property 'sequence_number_tmp' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(sequence_number_val));
		}
		sequence_number = std::move(sequence_number_tmp);
	}
	auto first_row_id_val = yyjson_obj_get(obj, "first-row-id");
	if (first_row_id_val) {
		int64_t first_row_id_tmp;
		if (yyjson_is_sint(first_row_id_val)) {
			first_row_id_tmp = yyjson_get_sint(first_row_id_val);
		} else if (yyjson_is_uint(first_row_id_val)) {
			first_row_id_tmp = yyjson_get_uint(first_row_id_val);
		} else {
			return StringUtil::Format(
			    "Snapshot property 'first_row_id_tmp' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(first_row_id_val));
		}
		first_row_id = std::move(first_row_id_tmp);
	}
	auto added_rows_val = yyjson_obj_get(obj, "added-rows");
	if (added_rows_val) {
		int64_t added_rows_tmp;
		if (yyjson_is_sint(added_rows_val)) {
			added_rows_tmp = yyjson_get_sint(added_rows_val);
		} else if (yyjson_is_uint(added_rows_val)) {
			added_rows_tmp = yyjson_get_uint(added_rows_val);
		} else {
			return StringUtil::Format("Snapshot property 'added_rows_tmp' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(added_rows_val));
		}
		added_rows = std::move(added_rows_tmp);
	}
	auto schema_id_val = yyjson_obj_get(obj, "schema-id");
	if (schema_id_val) {
		int32_t schema_id_tmp;
		if (yyjson_is_int(schema_id_val)) {
			schema_id_tmp = yyjson_get_int(schema_id_val);
		} else {
			return StringUtil::Format("Snapshot property 'schema_id_tmp' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(schema_id_val));
		}
		schema_id = std::move(schema_id_tmp);
	}
	return Validate();
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
