
#include "rest_catalog/objects/snapshot_log.hpp"

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

SnapshotLog::SnapshotLog(vector<Object3> value_p) : value(std::move(value_p)) {
}
SnapshotLog::Object3::Object3(int64_t snapshot_id_p, int64_t timestamp_ms_p)
    : snapshot_id(std::move(snapshot_id_p)), timestamp_ms(std::move(timestamp_ms_p)) {
}

SnapshotLog::Object3Builder::Object3Builder() {
}

SnapshotLog::Object3Builder &SnapshotLog::Object3Builder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

SnapshotLog::Object3Builder &SnapshotLog::Object3Builder::SetTimestampMs(int64_t value) {
	timestamp_ms_.emplace(std::move(value));
	has_timestamp_ms_ = true;
	return *this;
}

SnapshotLog::Object3 SnapshotLog::Object3Builder::Build() {
	if (!has_snapshot_id_) {
		throw InvalidInputException("Object3 required property 'snapshot-id' is missing");
	}
	if (!has_timestamp_ms_) {
		throw InvalidInputException("Object3 required property 'timestamp-ms' is missing");
	}
	auto result = SnapshotLog::Object3(std::move(*snapshot_id_), std::move(*timestamp_ms_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SnapshotLog::Object3Builder::TryBuild(optional<SnapshotLog::Object3> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotLog::Object3 SnapshotLog::Object3::FromJSON(yyjson_val *obj) {
	Object3Builder builder;
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		throw InvalidInputException("Object3 required property 'snapshot-id' is missing");
	} else {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("Object3 property 'snapshot_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
	if (!timestamp_ms_val) {
		throw InvalidInputException("Object3 required property 'timestamp-ms' is missing");
	} else {
		int64_t timestamp_ms;
		if (yyjson_is_sint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_sint(timestamp_ms_val);
		} else if (yyjson_is_uint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_uint(timestamp_ms_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("Object3 property 'timestamp_ms' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(timestamp_ms_val)));
		}
		builder.SetTimestampMs(std::move(timestamp_ms));
	}
	return builder.Build();
}

string SnapshotLog::Object3::TryFromJSON(yyjson_val *obj, optional<SnapshotLog::Object3> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotLog::Object3 SnapshotLog::Object3::Copy() const {
	Object3Builder builder;
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	int64_t timestamp_ms_tmp;
	timestamp_ms_tmp = timestamp_ms;
	builder.SetTimestampMs(std::move(timestamp_ms_tmp));
	return builder.Build();
}

string SnapshotLog::Object3::Validate() const {
	string error;
	return "";
}

void SnapshotLog::Object3::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: timestamp-ms
	yyjson_mut_obj_add_sint(doc, obj, "timestamp-ms", timestamp_ms);
}

yyjson_mut_val *SnapshotLog::Object3::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

SnapshotLog SnapshotLog::FromJSON(yyjson_val *obj) {
	vector<Object3> value;
	if (yyjson_is_arr(obj)) {
		size_t idx, max;
		yyjson_val *val;
		yyjson_arr_foreach(obj, idx, max, val) {
			auto tmp = Object3::FromJSON(val);
			value.emplace_back(std::move(tmp));
		}
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "SnapshotLog property 'value' is not of type 'array', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return SnapshotLog(std::move(value));
}

string SnapshotLog::TryFromJSON(yyjson_val *obj, optional<SnapshotLog> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotLog SnapshotLog::Copy() const {
	vector<Object3> value_tmp;
	value_tmp.reserve(value.size());
	for (auto &item : value) {
		value_tmp.emplace_back(item.Copy());
	}
	return SnapshotLog(std::move(value_tmp));
}

string SnapshotLog::Validate() const {
	string error;
	for (const auto &item : value) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

yyjson_mut_val *SnapshotLog::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *arr = yyjson_mut_arr(doc);
	for (const auto &item : value) {
		yyjson_mut_arr_append(arr, item.ToJSON(doc));
	}
	return arr;
}

} // namespace rest_api_objects
} // namespace duckdb
