
#include "rest_catalog/objects/file_scan_task.hpp"

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

FileScanTask::FileScanTask(DataFile data_file_p, optional<vector<int32_t>> delete_file_references_p,
                           unique_ptr<Expression> residual_filter_p)
    : data_file(std::move(data_file_p)), delete_file_references(std::move(delete_file_references_p)),
      residual_filter(std::move(residual_filter_p)) {
}
FileScanTask::FileScanTask(const FileScanTask &other)
    : data_file(other.data_file.Copy()),
      delete_file_references((other.delete_file_references.has_value() ? optional<vector<int32_t>>(([&]() {
	      vector<int32_t> copied;
	      copied.reserve((*other.delete_file_references).size());
	      for (const auto &item : (*other.delete_file_references)) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }()))
                                                                       : optional<vector<int32_t>>())),
      residual_filter(other.residual_filter ? make_uniq<Expression>(other.residual_filter->Copy()) : nullptr) {
}
FileScanTask::FileScanTask(FileScanTask &&other) : FileScanTask(static_cast<const FileScanTask &>(other)) {
}

FileScanTaskBuilder::FileScanTaskBuilder() {
}

FileScanTaskBuilder &FileScanTaskBuilder::SetDataFile(DataFile value) {
	data_file_.emplace(std::move(value));
	has_data_file_ = true;
	return *this;
}

FileScanTaskBuilder &FileScanTaskBuilder::SetDeleteFileReferences(vector<int32_t> value) {
	delete_file_references_.emplace(std::move(value));
	return *this;
}

FileScanTaskBuilder &FileScanTaskBuilder::SetResidualFilter(unique_ptr<Expression> value) {
	residual_filter_ = std::move(value);
	return *this;
}

FileScanTask FileScanTaskBuilder::Build() {
	if (!has_data_file_) {
		throw InvalidInputException("FileScanTask required property 'data-file' is missing");
	}
	auto result = FileScanTask(std::move(*data_file_), std::move(delete_file_references_), std::move(residual_filter_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> FileScanTaskBuilder::TryBuild(optional<FileScanTask> &result) {
	if (!has_data_file_) {
		return "FileScanTask required property 'data-file' is missing";
	}
	auto built = FileScanTask(std::move(*data_file_), std::move(delete_file_references_), std::move(residual_filter_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> FileScanTask::TryFromJSON(yyjson_val *obj, FileScanTaskBuilder &builder) {
	try {
		auto data_file_val = yyjson_obj_get(obj, "data-file");
		if (!data_file_val) {
			throw InvalidInputException("FileScanTask required property 'data-file' is missing");
		} else {
			builder.SetDataFile(DataFile::FromJSON(data_file_val));
		}
		auto delete_file_references_val = yyjson_obj_get(obj, "delete-file-references");
		if (delete_file_references_val) {
			vector<int32_t> delete_file_references;
			if (yyjson_is_arr(delete_file_references_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(delete_file_references_val, idx, max, val) {
					int32_t tmp;
					if (yyjson_is_int(val)) {
						tmp = yyjson_get_int(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "FileScanTask property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					delete_file_references.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "FileScanTask property 'delete_file_references' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(delete_file_references_val)));
			}
			builder.SetDeleteFileReferences(std::move(delete_file_references));
		}
		auto residual_filter_val = yyjson_obj_get(obj, "residual-filter");
		if (residual_filter_val) {
			unique_ptr<Expression> residual_filter;
			residual_filter = make_uniq<Expression>(Expression::FromJSON(residual_filter_val));
			builder.SetResidualFilter(std::move(residual_filter));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FileScanTask FileScanTask::FromJSON(yyjson_val *obj) {
	FileScanTaskBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

FileScanTask FileScanTask::Copy() const {
	return FileScanTask(*this);
}

optional<string> FileScanTask::Validate() const {
	optional<string> error;
	error = data_file.Validate();
	if (error) {
		return error;
	}
	if (residual_filter != nullptr) {
		error = residual_filter->Validate();
		if (error) {
			return error;
		}
	}
	return nullopt;
}

void FileScanTask::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: data-file
	yyjson_mut_val *data_file_val = data_file.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "data-file", data_file_val);

	// Serialize: delete-file-references
	if (delete_file_references.has_value()) {
		auto &delete_file_references_value = *delete_file_references;
		yyjson_mut_val *delete_file_references_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : delete_file_references_value) {
			yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
			yyjson_mut_arr_append(delete_file_references_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "delete-file-references", delete_file_references_value_arr);
	}

	// Serialize: residual-filter
	if (residual_filter != nullptr) {
		yyjson_mut_val *residual_filter_val = residual_filter->ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "residual-filter", residual_filter_val);
	}
}

yyjson_mut_val *FileScanTask::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
