
#include "rest_catalog/objects/content_file.hpp"

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

ContentFile::ContentFile(int32_t spec_id_p, vector<PrimitiveTypeValue> partition_p, string content_p,
                         string file_path_p, FileFormat file_format_p, int64_t file_size_in_bytes_p,
                         int64_t record_count_p, optional<BinaryTypeValue> key_metadata_p,
                         optional<vector<int64_t>> split_offsets_p, optional<int32_t> sort_order_id_p)
    : spec_id(std::move(spec_id_p)), partition(std::move(partition_p)), content(std::move(content_p)),
      file_path(std::move(file_path_p)), file_format(std::move(file_format_p)),
      file_size_in_bytes(std::move(file_size_in_bytes_p)), record_count(std::move(record_count_p)),
      key_metadata(std::move(key_metadata_p)), split_offsets(std::move(split_offsets_p)),
      sort_order_id(std::move(sort_order_id_p)) {
}

ContentFileBuilder::ContentFileBuilder() {
}

ContentFileBuilder &ContentFileBuilder::SetSpecId(int32_t value) {
	spec_id_.emplace(std::move(value));
	has_spec_id_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetPartition(vector<PrimitiveTypeValue> value) {
	partition_.emplace(std::move(value));
	has_partition_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetContent(string value) {
	content_.emplace(std::move(value));
	has_content_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetFilePath(string value) {
	file_path_.emplace(std::move(value));
	has_file_path_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetFileFormat(FileFormat value) {
	file_format_.emplace(std::move(value));
	has_file_format_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetFileSizeInBytes(int64_t value) {
	file_size_in_bytes_.emplace(std::move(value));
	has_file_size_in_bytes_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetRecordCount(int64_t value) {
	record_count_.emplace(std::move(value));
	has_record_count_ = true;
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetKeyMetadata(BinaryTypeValue value) {
	key_metadata_.emplace(std::move(value));
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetSplitOffsets(vector<int64_t> value) {
	split_offsets_.emplace(std::move(value));
	return *this;
}

ContentFileBuilder &ContentFileBuilder::SetSortOrderId(int32_t value) {
	sort_order_id_.emplace(std::move(value));
	return *this;
}

ContentFile ContentFileBuilder::Build() {
	if (!has_spec_id_) {
		throw InvalidInputException("ContentFile required property 'spec-id' is missing");
	}
	if (!has_partition_) {
		throw InvalidInputException("ContentFile required property 'partition' is missing");
	}
	if (!has_content_) {
		throw InvalidInputException("ContentFile required property 'content' is missing");
	}
	if (!has_file_path_) {
		throw InvalidInputException("ContentFile required property 'file-path' is missing");
	}
	if (!has_file_format_) {
		throw InvalidInputException("ContentFile required property 'file-format' is missing");
	}
	if (!has_file_size_in_bytes_) {
		throw InvalidInputException("ContentFile required property 'file-size-in-bytes' is missing");
	}
	if (!has_record_count_) {
		throw InvalidInputException("ContentFile required property 'record-count' is missing");
	}
	auto result =
	    ContentFile(std::move(*spec_id_), std::move(*partition_), std::move(*content_), std::move(*file_path_),
	                std::move(*file_format_), std::move(*file_size_in_bytes_), std::move(*record_count_),
	                std::move(key_metadata_), std::move(split_offsets_), std::move(sort_order_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ContentFileBuilder::TryBuild(optional<ContentFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ContentFile ContentFile::FromJSON(yyjson_val *obj) {
	ContentFileBuilder builder;
	auto spec_id_val = yyjson_obj_get(obj, "spec-id");
	if (!spec_id_val) {
		throw InvalidInputException("ContentFile required property 'spec-id' is missing");
	} else {
		int32_t spec_id;
		if (yyjson_is_int(spec_id_val)) {
			spec_id = yyjson_get_int(spec_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'spec_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(spec_id_val)));
		}
		builder.SetSpecId(std::move(spec_id));
	}
	auto partition_val = yyjson_obj_get(obj, "partition");
	if (!partition_val) {
		throw InvalidInputException("ContentFile required property 'partition' is missing");
	} else {
		vector<PrimitiveTypeValue> partition;
		if (yyjson_is_arr(partition_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(partition_val, idx, max, val) {
				auto tmp = PrimitiveTypeValue::FromJSON(val);
				partition.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'partition' is not of type 'array', found '%s' instead",
			                       yyjson_get_type_desc(partition_val)));
		}
		builder.SetPartition(std::move(partition));
	}
	auto content_val = yyjson_obj_get(obj, "content");
	if (!content_val) {
		throw InvalidInputException("ContentFile required property 'content' is missing");
	} else {
		string content;
		if (yyjson_is_str(content_val)) {
			content = yyjson_get_str(content_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'content' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(content_val)));
		}
		builder.SetContent(std::move(content));
	}
	auto file_path_val = yyjson_obj_get(obj, "file-path");
	if (!file_path_val) {
		throw InvalidInputException("ContentFile required property 'file-path' is missing");
	} else {
		string file_path;
		if (yyjson_is_str(file_path_val)) {
			file_path = yyjson_get_str(file_path_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'file_path' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(file_path_val)));
		}
		builder.SetFilePath(std::move(file_path));
	}
	auto file_format_val = yyjson_obj_get(obj, "file-format");
	if (!file_format_val) {
		throw InvalidInputException("ContentFile required property 'file-format' is missing");
	} else {
		builder.SetFileFormat(FileFormat::FromJSON(file_format_val));
	}
	auto file_size_in_bytes_val = yyjson_obj_get(obj, "file-size-in-bytes");
	if (!file_size_in_bytes_val) {
		throw InvalidInputException("ContentFile required property 'file-size-in-bytes' is missing");
	} else {
		int64_t file_size_in_bytes;
		if (yyjson_is_sint(file_size_in_bytes_val)) {
			file_size_in_bytes = yyjson_get_sint(file_size_in_bytes_val);
		} else if (yyjson_is_uint(file_size_in_bytes_val)) {
			file_size_in_bytes = yyjson_get_uint(file_size_in_bytes_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "ContentFile property 'file_size_in_bytes' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(file_size_in_bytes_val)));
		}
		builder.SetFileSizeInBytes(std::move(file_size_in_bytes));
	}
	auto record_count_val = yyjson_obj_get(obj, "record-count");
	if (!record_count_val) {
		throw InvalidInputException("ContentFile required property 'record-count' is missing");
	} else {
		int64_t record_count;
		if (yyjson_is_sint(record_count_val)) {
			record_count = yyjson_get_sint(record_count_val);
		} else if (yyjson_is_uint(record_count_val)) {
			record_count = yyjson_get_uint(record_count_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'record_count' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(record_count_val)));
		}
		builder.SetRecordCount(std::move(record_count));
	}
	auto key_metadata_val = yyjson_obj_get(obj, "key-metadata");
	if (key_metadata_val) {
		builder.SetKeyMetadata(BinaryTypeValue::FromJSON(key_metadata_val));
	}
	auto split_offsets_val = yyjson_obj_get(obj, "split-offsets");
	if (split_offsets_val) {
		vector<int64_t> split_offsets;
		if (yyjson_is_arr(split_offsets_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(split_offsets_val, idx, max, val) {
				int64_t tmp;
				if (yyjson_is_sint(val)) {
					tmp = yyjson_get_sint(val);
				} else if (yyjson_is_uint(val)) {
					tmp = yyjson_get_uint(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("ContentFile property 'tmp' is not of type 'integer', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				split_offsets.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'split_offsets' is not of type 'array', found '%s' instead",
			                       yyjson_get_type_desc(split_offsets_val)));
		}
		builder.SetSplitOffsets(std::move(split_offsets));
	}
	auto sort_order_id_val = yyjson_obj_get(obj, "sort-order-id");
	if (sort_order_id_val) {
		int32_t sort_order_id;
		if (yyjson_is_int(sort_order_id_val)) {
			sort_order_id = yyjson_get_int(sort_order_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ContentFile property 'sort_order_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(sort_order_id_val)));
		}
		builder.SetSortOrderId(std::move(sort_order_id));
	}
	return builder.Build();
}

string ContentFile::TryFromJSON(yyjson_val *obj, optional<ContentFile> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ContentFile ContentFile::Copy() const {
	ContentFileBuilder builder;
	int32_t spec_id_tmp;
	spec_id_tmp = spec_id;
	builder.SetSpecId(std::move(spec_id_tmp));
	vector<PrimitiveTypeValue> partition_tmp;
	partition_tmp.reserve(partition.size());
	for (auto &item : partition) {
		partition_tmp.emplace_back(item.Copy());
	}
	builder.SetPartition(std::move(partition_tmp));
	string content_tmp;
	content_tmp = content;
	builder.SetContent(std::move(content_tmp));
	string file_path_tmp;
	file_path_tmp = file_path;
	builder.SetFilePath(std::move(file_path_tmp));
	auto file_format_tmp = file_format.Copy();
	builder.SetFileFormat(std::move(file_format_tmp));
	int64_t file_size_in_bytes_tmp;
	file_size_in_bytes_tmp = file_size_in_bytes;
	builder.SetFileSizeInBytes(std::move(file_size_in_bytes_tmp));
	int64_t record_count_tmp;
	record_count_tmp = record_count;
	builder.SetRecordCount(std::move(record_count_tmp));
	optional<BinaryTypeValue> key_metadata_tmp;
	if (key_metadata.has_value()) {
		key_metadata_tmp.emplace((*key_metadata).Copy());
	}
	if (key_metadata_tmp.has_value()) {
		builder.SetKeyMetadata(std::move(*key_metadata_tmp));
	}
	optional<vector<int64_t>> split_offsets_tmp;
	if (split_offsets.has_value()) {
		split_offsets_tmp.emplace();
		(*split_offsets_tmp).reserve((*split_offsets).size());
		for (auto &item : (*split_offsets)) {
			(*split_offsets_tmp).emplace_back(item);
		}
	}
	if (split_offsets_tmp.has_value()) {
		builder.SetSplitOffsets(std::move((*split_offsets_tmp)));
	}
	optional<int32_t> sort_order_id_tmp;
	if (sort_order_id.has_value()) {
		sort_order_id_tmp.emplace();
		(*sort_order_id_tmp) = (*sort_order_id);
	}
	if (sort_order_id_tmp.has_value()) {
		builder.SetSortOrderId(std::move((*sort_order_id_tmp)));
	}
	return builder.Build();
}

string ContentFile::Validate() const {
	string error;
	for (const auto &item : partition) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	error = file_format.Validate();
	if (!error.empty()) {
		return error;
	}
	if (key_metadata.has_value()) {
		error = (*key_metadata).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void ContentFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: spec-id
	yyjson_mut_obj_add_int(doc, obj, "spec-id", spec_id);

	// Serialize: partition
	yyjson_mut_val *partition_arr = yyjson_mut_arr(doc);
	for (const auto &item : partition) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(partition_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "partition", partition_arr);

	// Serialize: content
	yyjson_mut_obj_add_strcpy(doc, obj, "content", content.c_str());

	// Serialize: file-path
	yyjson_mut_obj_add_strcpy(doc, obj, "file-path", file_path.c_str());

	// Serialize: file-format
	yyjson_mut_val *file_format_val = file_format.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "file-format", file_format_val);

	// Serialize: file-size-in-bytes
	yyjson_mut_obj_add_sint(doc, obj, "file-size-in-bytes", file_size_in_bytes);

	// Serialize: record-count
	yyjson_mut_obj_add_sint(doc, obj, "record-count", record_count);

	// Serialize: key-metadata
	if (key_metadata.has_value()) {
		auto &key_metadata_value = *key_metadata;
		yyjson_mut_val *key_metadata_value_val = key_metadata_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "key-metadata", key_metadata_value_val);
	}

	// Serialize: split-offsets
	if (split_offsets.has_value()) {
		auto &split_offsets_value = *split_offsets;
		yyjson_mut_val *split_offsets_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : split_offsets_value) {
			yyjson_mut_val *item_val = yyjson_mut_sint(doc, item);
			yyjson_mut_arr_append(split_offsets_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "split-offsets", split_offsets_value_arr);
	}

	// Serialize: sort-order-id
	if (sort_order_id.has_value()) {
		auto &sort_order_id_value = *sort_order_id;
		yyjson_mut_obj_add_int(doc, obj, "sort-order-id", sort_order_id_value);
	}
}

yyjson_mut_val *ContentFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
