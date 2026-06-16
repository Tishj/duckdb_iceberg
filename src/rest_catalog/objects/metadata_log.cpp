
#include "rest_catalog/objects/metadata_log.hpp"

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

MetadataLog::MetadataLog(vector<Object4> value_p) : value(std::move(value_p)) {
}
MetadataLog::Object4::Object4(string metadata_file_p, int64_t timestamp_ms_p)
    : metadata_file(std::move(metadata_file_p)), timestamp_ms(std::move(timestamp_ms_p)) {
}

MetadataLog::Object4Builder::Object4Builder() {
}

MetadataLog::Object4Builder &MetadataLog::Object4Builder::SetMetadataFile(string value) {
	metadata_file_.emplace(std::move(value));
	has_metadata_file_ = true;
	return *this;
}

MetadataLog::Object4Builder &MetadataLog::Object4Builder::SetTimestampMs(int64_t value) {
	timestamp_ms_.emplace(std::move(value));
	has_timestamp_ms_ = true;
	return *this;
}

MetadataLog::Object4 MetadataLog::Object4Builder::Build() {
	if (!has_metadata_file_) {
		throw InvalidInputException("Object4 required property 'metadata-file' is missing");
	}
	if (!has_timestamp_ms_) {
		throw InvalidInputException("Object4 required property 'timestamp-ms' is missing");
	}
	auto result = MetadataLog::Object4(std::move(*metadata_file_), std::move(*timestamp_ms_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string MetadataLog::Object4Builder::TryBuild(optional<MetadataLog::Object4> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MetadataLog::Object4 MetadataLog::Object4::FromJSON(yyjson_val *obj) {
	Object4Builder builder;
	auto metadata_file_val = yyjson_obj_get(obj, "metadata-file");
	if (!metadata_file_val) {
		throw InvalidInputException("Object4 required property 'metadata-file' is missing");
	} else {
		string metadata_file;
		if (yyjson_is_str(metadata_file_val)) {
			metadata_file = yyjson_get_str(metadata_file_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("Object4 property 'metadata_file' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(metadata_file_val)));
		}
		builder.SetMetadataFile(std::move(metadata_file));
	}
	auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
	if (!timestamp_ms_val) {
		throw InvalidInputException("Object4 required property 'timestamp-ms' is missing");
	} else {
		int64_t timestamp_ms;
		if (yyjson_is_sint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_sint(timestamp_ms_val);
		} else if (yyjson_is_uint(timestamp_ms_val)) {
			timestamp_ms = yyjson_get_uint(timestamp_ms_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("Object4 property 'timestamp_ms' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(timestamp_ms_val)));
		}
		builder.SetTimestampMs(std::move(timestamp_ms));
	}
	return builder.Build();
}

string MetadataLog::Object4::TryFromJSON(yyjson_val *obj, optional<MetadataLog::Object4> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MetadataLog::Object4 MetadataLog::Object4::Copy() const {
	Object4Builder builder;
	string metadata_file_tmp;
	metadata_file_tmp = metadata_file;
	builder.SetMetadataFile(std::move(metadata_file_tmp));
	int64_t timestamp_ms_tmp;
	timestamp_ms_tmp = timestamp_ms;
	builder.SetTimestampMs(std::move(timestamp_ms_tmp));
	return builder.Build();
}

string MetadataLog::Object4::Validate() const {
	string error;
	return "";
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
	vector<Object4> value;
	if (yyjson_is_arr(obj)) {
		size_t idx, max;
		yyjson_val *val;
		yyjson_arr_foreach(obj, idx, max, val) {
			auto tmp = Object4::FromJSON(val);
			value.emplace_back(std::move(tmp));
		}
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "MetadataLog property 'value' is not of type 'array', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return MetadataLog(std::move(value));
}

string MetadataLog::TryFromJSON(yyjson_val *obj, optional<MetadataLog> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

MetadataLog MetadataLog::Copy() const {
	vector<Object4> value_tmp;
	value_tmp.reserve(value.size());
	for (auto &item : value) {
		value_tmp.emplace_back(item.Copy());
	}
	return MetadataLog(std::move(value_tmp));
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

yyjson_mut_val *MetadataLog::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *arr = yyjson_mut_arr(doc);
	for (const auto &item : value) {
		yyjson_mut_arr_append(arr, item.ToJSON(doc));
	}
	return arr;
}

} // namespace rest_api_objects
} // namespace duckdb
