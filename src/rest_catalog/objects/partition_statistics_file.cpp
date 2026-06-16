
#include "rest_catalog/objects/partition_statistics_file.hpp"

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

PartitionStatisticsFile::PartitionStatisticsFile(int64_t snapshot_id_p, string statistics_path_p,
                                                 int64_t file_size_in_bytes_p)
    : snapshot_id(std::move(snapshot_id_p)), statistics_path(std::move(statistics_path_p)),
      file_size_in_bytes(std::move(file_size_in_bytes_p)) {
}

PartitionStatisticsFileBuilder::PartitionStatisticsFileBuilder() {
}

PartitionStatisticsFileBuilder &PartitionStatisticsFileBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

PartitionStatisticsFileBuilder &PartitionStatisticsFileBuilder::SetStatisticsPath(string value) {
	statistics_path_.emplace(std::move(value));
	has_statistics_path_ = true;
	return *this;
}

PartitionStatisticsFileBuilder &PartitionStatisticsFileBuilder::SetFileSizeInBytes(int64_t value) {
	file_size_in_bytes_.emplace(std::move(value));
	has_file_size_in_bytes_ = true;
	return *this;
}

PartitionStatisticsFile PartitionStatisticsFileBuilder::Build() {
	if (!has_snapshot_id_) {
		throw InvalidInputException("PartitionStatisticsFile required property 'snapshot-id' is missing");
	}
	if (!has_statistics_path_) {
		throw InvalidInputException("PartitionStatisticsFile required property 'statistics-path' is missing");
	}
	if (!has_file_size_in_bytes_) {
		throw InvalidInputException("PartitionStatisticsFile required property 'file-size-in-bytes' is missing");
	}
	auto result = PartitionStatisticsFile(std::move(*snapshot_id_), std::move(*statistics_path_),
	                                      std::move(*file_size_in_bytes_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PartitionStatisticsFileBuilder::TryBuild(optional<PartitionStatisticsFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string PartitionStatisticsFile::TryFromJSON(yyjson_val *obj, PartitionStatisticsFileBuilder &builder) {
	try {
		auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
		if (!snapshot_id_val) {
			throw InvalidInputException("PartitionStatisticsFile required property 'snapshot-id' is missing");
		} else {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PartitionStatisticsFile property 'snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
		auto statistics_path_val = yyjson_obj_get(obj, "statistics-path");
		if (!statistics_path_val) {
			throw InvalidInputException("PartitionStatisticsFile required property 'statistics-path' is missing");
		} else {
			string statistics_path;
			if (yyjson_is_str(statistics_path_val)) {
				statistics_path = yyjson_get_str(statistics_path_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PartitionStatisticsFile property 'statistics_path' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(statistics_path_val)));
			}
			builder.SetStatisticsPath(std::move(statistics_path));
		}
		auto file_size_in_bytes_val = yyjson_obj_get(obj, "file-size-in-bytes");
		if (!file_size_in_bytes_val) {
			throw InvalidInputException("PartitionStatisticsFile required property 'file-size-in-bytes' is missing");
		} else {
			int64_t file_size_in_bytes;
			if (yyjson_is_sint(file_size_in_bytes_val)) {
				file_size_in_bytes = yyjson_get_sint(file_size_in_bytes_val);
			} else if (yyjson_is_uint(file_size_in_bytes_val)) {
				file_size_in_bytes = yyjson_get_uint(file_size_in_bytes_val);
			} else {
				throw InvalidInputException(StringUtil::Format("PartitionStatisticsFile property 'file_size_in_bytes' "
				                                               "is not of type 'integer', found '%s' instead",
				                                               yyjson_get_type_desc(file_size_in_bytes_val)));
			}
			builder.SetFileSizeInBytes(std::move(file_size_in_bytes));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PartitionStatisticsFile PartitionStatisticsFile::FromJSON(yyjson_val *obj) {
	PartitionStatisticsFileBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

PartitionStatisticsFile PartitionStatisticsFile::Copy() const {
	PartitionStatisticsFileBuilder builder;
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	string statistics_path_tmp;
	statistics_path_tmp = statistics_path;
	builder.SetStatisticsPath(std::move(statistics_path_tmp));
	int64_t file_size_in_bytes_tmp;
	file_size_in_bytes_tmp = file_size_in_bytes;
	builder.SetFileSizeInBytes(std::move(file_size_in_bytes_tmp));
	return builder.Build();
}

string PartitionStatisticsFile::Validate() const {
	string error;
	return "";
}

void PartitionStatisticsFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: statistics-path
	yyjson_mut_obj_add_strcpy(doc, obj, "statistics-path", statistics_path.c_str());

	// Serialize: file-size-in-bytes
	yyjson_mut_obj_add_sint(doc, obj, "file-size-in-bytes", file_size_in_bytes);
}

yyjson_mut_val *PartitionStatisticsFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
