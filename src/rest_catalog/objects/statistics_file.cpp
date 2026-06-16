
#include "rest_catalog/objects/statistics_file.hpp"

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

StatisticsFile::StatisticsFile(int64_t snapshot_id_p, string statistics_path_p, int64_t file_size_in_bytes_p,
                               int64_t file_footer_size_in_bytes_p, vector<BlobMetadata> blob_metadata_p)
    : snapshot_id(std::move(snapshot_id_p)), statistics_path(std::move(statistics_path_p)),
      file_size_in_bytes(std::move(file_size_in_bytes_p)),
      file_footer_size_in_bytes(std::move(file_footer_size_in_bytes_p)), blob_metadata(std::move(blob_metadata_p)) {
}
StatisticsFile::StatisticsFile(const StatisticsFile &other)
    : snapshot_id(other.snapshot_id), statistics_path(other.statistics_path),
      file_size_in_bytes(other.file_size_in_bytes), file_footer_size_in_bytes(other.file_footer_size_in_bytes),
      blob_metadata(([&]() {
	      vector<BlobMetadata> copied;
	      copied.reserve(other.blob_metadata.size());
	      for (const auto &item : other.blob_metadata) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }())) {
}
StatisticsFile::StatisticsFile(StatisticsFile &&other) : StatisticsFile(static_cast<const StatisticsFile &>(other)) {
}

StatisticsFileBuilder::StatisticsFileBuilder() {
}

StatisticsFileBuilder &StatisticsFileBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

StatisticsFileBuilder &StatisticsFileBuilder::SetStatisticsPath(string value) {
	statistics_path_.emplace(std::move(value));
	has_statistics_path_ = true;
	return *this;
}

StatisticsFileBuilder &StatisticsFileBuilder::SetFileSizeInBytes(int64_t value) {
	file_size_in_bytes_.emplace(std::move(value));
	has_file_size_in_bytes_ = true;
	return *this;
}

StatisticsFileBuilder &StatisticsFileBuilder::SetFileFooterSizeInBytes(int64_t value) {
	file_footer_size_in_bytes_.emplace(std::move(value));
	has_file_footer_size_in_bytes_ = true;
	return *this;
}

StatisticsFileBuilder &StatisticsFileBuilder::SetBlobMetadata(vector<BlobMetadata> value) {
	blob_metadata_.emplace(std::move(value));
	has_blob_metadata_ = true;
	return *this;
}

StatisticsFile StatisticsFileBuilder::Build() {
	if (!has_snapshot_id_) {
		throw InvalidInputException("StatisticsFile required property 'snapshot-id' is missing");
	}
	if (!has_statistics_path_) {
		throw InvalidInputException("StatisticsFile required property 'statistics-path' is missing");
	}
	if (!has_file_size_in_bytes_) {
		throw InvalidInputException("StatisticsFile required property 'file-size-in-bytes' is missing");
	}
	if (!has_file_footer_size_in_bytes_) {
		throw InvalidInputException("StatisticsFile required property 'file-footer-size-in-bytes' is missing");
	}
	if (!has_blob_metadata_) {
		throw InvalidInputException("StatisticsFile required property 'blob-metadata' is missing");
	}
	auto result =
	    StatisticsFile(std::move(*snapshot_id_), std::move(*statistics_path_), std::move(*file_size_in_bytes_),
	                   std::move(*file_footer_size_in_bytes_), std::move(*blob_metadata_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> StatisticsFileBuilder::TryBuild(optional<StatisticsFile> &result) {
	if (!has_snapshot_id_) {
		return "StatisticsFile required property 'snapshot-id' is missing";
	}
	if (!has_statistics_path_) {
		return "StatisticsFile required property 'statistics-path' is missing";
	}
	if (!has_file_size_in_bytes_) {
		return "StatisticsFile required property 'file-size-in-bytes' is missing";
	}
	if (!has_file_footer_size_in_bytes_) {
		return "StatisticsFile required property 'file-footer-size-in-bytes' is missing";
	}
	if (!has_blob_metadata_) {
		return "StatisticsFile required property 'blob-metadata' is missing";
	}
	auto built = StatisticsFile(std::move(*snapshot_id_), std::move(*statistics_path_), std::move(*file_size_in_bytes_),
	                            std::move(*file_footer_size_in_bytes_), std::move(*blob_metadata_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> StatisticsFile::TryFromJSON(yyjson_val *obj, StatisticsFileBuilder &builder) {
	try {
		auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
		if (!snapshot_id_val) {
			throw InvalidInputException("StatisticsFile required property 'snapshot-id' is missing");
		} else {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StatisticsFile property 'snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
		auto statistics_path_val = yyjson_obj_get(obj, "statistics-path");
		if (!statistics_path_val) {
			throw InvalidInputException("StatisticsFile required property 'statistics-path' is missing");
		} else {
			string statistics_path;
			if (yyjson_is_str(statistics_path_val)) {
				statistics_path = yyjson_get_str(statistics_path_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StatisticsFile property 'statistics_path' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(statistics_path_val)));
			}
			builder.SetStatisticsPath(std::move(statistics_path));
		}
		auto file_size_in_bytes_val = yyjson_obj_get(obj, "file-size-in-bytes");
		if (!file_size_in_bytes_val) {
			throw InvalidInputException("StatisticsFile required property 'file-size-in-bytes' is missing");
		} else {
			int64_t file_size_in_bytes;
			if (yyjson_is_sint(file_size_in_bytes_val)) {
				file_size_in_bytes = yyjson_get_sint(file_size_in_bytes_val);
			} else if (yyjson_is_uint(file_size_in_bytes_val)) {
				file_size_in_bytes = yyjson_get_uint(file_size_in_bytes_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StatisticsFile property 'file_size_in_bytes' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(file_size_in_bytes_val)));
			}
			builder.SetFileSizeInBytes(std::move(file_size_in_bytes));
		}
		auto file_footer_size_in_bytes_val = yyjson_obj_get(obj, "file-footer-size-in-bytes");
		if (!file_footer_size_in_bytes_val) {
			throw InvalidInputException("StatisticsFile required property 'file-footer-size-in-bytes' is missing");
		} else {
			int64_t file_footer_size_in_bytes;
			if (yyjson_is_sint(file_footer_size_in_bytes_val)) {
				file_footer_size_in_bytes = yyjson_get_sint(file_footer_size_in_bytes_val);
			} else if (yyjson_is_uint(file_footer_size_in_bytes_val)) {
				file_footer_size_in_bytes = yyjson_get_uint(file_footer_size_in_bytes_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StatisticsFile property 'file_footer_size_in_bytes' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(file_footer_size_in_bytes_val)));
			}
			builder.SetFileFooterSizeInBytes(std::move(file_footer_size_in_bytes));
		}
		auto blob_metadata_val = yyjson_obj_get(obj, "blob-metadata");
		if (!blob_metadata_val) {
			throw InvalidInputException("StatisticsFile required property 'blob-metadata' is missing");
		} else {
			vector<BlobMetadata> blob_metadata;
			if (yyjson_is_arr(blob_metadata_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(blob_metadata_val, idx, max, val) {
					auto tmp = BlobMetadata::FromJSON(val);
					blob_metadata.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "StatisticsFile property 'blob_metadata' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(blob_metadata_val)));
			}
			builder.SetBlobMetadata(std::move(blob_metadata));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

StatisticsFile StatisticsFile::FromJSON(yyjson_val *obj) {
	StatisticsFileBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

StatisticsFile StatisticsFile::Copy() const {
	return StatisticsFile(*this);
}

optional<string> StatisticsFile::Validate() const {
	optional<string> error;
	for (const auto &item : blob_metadata) {
		error = item.Validate();
		if (error) {
			return error;
		}
	}
	return nullopt;
}

void StatisticsFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: statistics-path
	yyjson_mut_obj_add_strcpy(doc, obj, "statistics-path", statistics_path.c_str());

	// Serialize: file-size-in-bytes
	yyjson_mut_obj_add_sint(doc, obj, "file-size-in-bytes", file_size_in_bytes);

	// Serialize: file-footer-size-in-bytes
	yyjson_mut_obj_add_sint(doc, obj, "file-footer-size-in-bytes", file_footer_size_in_bytes);

	// Serialize: blob-metadata
	yyjson_mut_val *blob_metadata_arr = yyjson_mut_arr(doc);
	for (const auto &item : blob_metadata) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(blob_metadata_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "blob-metadata", blob_metadata_arr);
}

yyjson_mut_val *StatisticsFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
