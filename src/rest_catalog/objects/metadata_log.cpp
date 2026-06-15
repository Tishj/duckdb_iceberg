
#include "rest_catalog/objects/metadata_log.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

MetadataLog::MetadataLog() {
}
MetadataLog::Object4::Object4() {
}

MetadataLog::Object4Builder::Object4Builder() {
}

MetadataLog::Object4Builder &MetadataLog::Object4Builder::SetMetadataFile(string value) {
	result_.metadata_file = std::move(value);
	has_metadata_file_ = true;
	return *this;
}

MetadataLog::Object4Builder &MetadataLog::Object4Builder::SetTimestampMs(int64_t value) {
	result_.timestamp_ms = std::move(value);
	has_timestamp_ms_ = true;
	return *this;
}

string MetadataLog::Object4Builder::TryBuild(MetadataLog::Object4 &result) {
	if (!has_metadata_file_) {
		return "Object4 required property 'metadata-file' is missing";
	}
	if (!has_timestamp_ms_) {
		return "Object4 required property 'timestamp-ms' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

MetadataLog::Object4 MetadataLog::Object4Builder::Build() {
	MetadataLog::Object4 result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

MetadataLog::Object4 MetadataLog::Object4::FromJSON(yyjson_val *obj) {
	Object4 res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

MetadataLog::Object4 MetadataLog::Object4::Copy() const {
	Object4 res;
	res.metadata_file = metadata_file;
	res.timestamp_ms = timestamp_ms;
	return res;
}

string MetadataLog::Object4::Validate() const {
	string error;
	return "";
}

string MetadataLog::Object4::TryFromJSON(yyjson_val *obj) {
	string error;
	auto metadata_file_val = yyjson_obj_get(obj, "metadata-file");
	if (!metadata_file_val) {
		return "Object4 required property 'metadata-file' is missing";
	} else {
		if (yyjson_is_str(metadata_file_val)) {
			metadata_file = yyjson_get_str(metadata_file_val);
		} else {
			return StringUtil::Format("Object4 property 'metadata_file' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(metadata_file_val));
		}
	}
	auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
	if (!timestamp_ms_val) {
		return "Object4 required property 'timestamp-ms' is missing";
	} else {
		if (yyjson_is_sint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_sint(timestamp_ms_val);
		} else if (yyjson_is_uint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_uint(timestamp_ms_val);
		} else {
			return StringUtil::Format("Object4 property 'timestamp_ms' is not of type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(timestamp_ms_val));
		}
	}
	return Validate();
}

void MetadataLog::Object4::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: metadata-file
	yyjson_mut_obj_add_strcpy(doc, obj, "metadata-file", metadata_file.c_str());

	// Serialize: timestamp-ms
	yyjson_mut_obj_add_sint(doc, obj, "timestamp-ms", timestamp_ms);
}

yyjson_mut_val *MetadataLog::Object4::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

MetadataLog MetadataLog::FromJSON(yyjson_val *obj) {
	MetadataLog res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

MetadataLog MetadataLog::Copy() const {
	MetadataLog res;
	res.value.reserve(value.size());
	for (auto &item : value) {
		res.value.emplace_back(item.Copy());
	}
	return res;
}

string MetadataLog::Validate() const {
	string error;
	for (const auto &item : value) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

string MetadataLog::TryFromJSON(yyjson_val *obj) {
	string error;
	if (yyjson_is_arr(obj)) {
		size_t idx, max;
		yyjson_val *val;
		yyjson_arr_foreach(obj, idx, max, val) {
			auto tmp = GeneratedObjectAccess::Create<Object4>();
			error = tmp.TryFromJSON(val);
			if (!error.empty()) {
				return error;
			}
			value.emplace_back(std::move(tmp));
		}
	} else {
		return StringUtil::Format("MetadataLog property 'value' is not of type 'array', found '%s' instead",
		                          yyjson_get_type_desc(obj));
	}
	return Validate();
}

yyjson_mut_val *MetadataLog::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *arr = yyjson_mut_arr(doc);
	for (const auto &item : value) {
		yyjson_mut_arr_append(arr, item.ToJSON(doc));
	}
	return arr;
}

} // namespace rest_api_objects
} // namespace duckdb
