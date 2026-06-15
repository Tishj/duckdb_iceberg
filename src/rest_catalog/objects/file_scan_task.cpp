
#include "rest_catalog/objects/file_scan_task.hpp"

#include <regex>

#include "yyjson.hpp"
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

FileScanTaskBuilder::FileScanTaskBuilder() {
}

FileScanTaskBuilder &FileScanTaskBuilder::SetDataFile(DataFile value) {
	data_file_ = std::move(value);
	has_data_file_ = true;
	return *this;
}

FileScanTaskBuilder &FileScanTaskBuilder::SetDeleteFileReferences(vector<int32_t> value) {
	delete_file_references_ = std::move(value);
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
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string FileScanTaskBuilder::TryBuild(optional<FileScanTask> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FileScanTask FileScanTask::FromJSON(yyjson_val *obj) {
	FileScanTaskBuilder builder;
	auto data_file_val = yyjson_obj_get(obj, "data-file");
	if (!data_file_val) {
		throw InvalidInputException("FileScanTask required property 'data-file' is missing");
	} else {
		optional<DataFile> data_file;
		data_file = DataFile::FromJSON(data_file_val);
		builder.SetDataFile(std::move(*data_file));
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
					throw InvalidInputException(
					    StringUtil::Format("FileScanTask property 'tmp' is not of type 'integer', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				delete_file_references.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "FileScanTask property 'delete_file_references' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(delete_file_references_val));
		}
		builder.SetDeleteFileReferences(std::move(delete_file_references));
	}
	auto residual_filter_val = yyjson_obj_get(obj, "residual-filter");
	if (residual_filter_val) {
		unique_ptr<Expression> residual_filter;
		residual_filter = make_uniq<Expression>(Expression::FromJSON(residual_filter_val));
		builder.SetResidualFilter(std::move(residual_filter));
	}
	return builder.Build();
}

string FileScanTask::TryFromJSON(yyjson_val *obj, optional<FileScanTask> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FileScanTask FileScanTask::Copy() const {
	FileScanTaskBuilder builder;
	optional<DataFile> data_file_tmp;
	data_file_tmp = data_file.Copy();
	builder.SetDataFile(std::move(*data_file_tmp));
	vector<int32_t> delete_file_references_tmp;
	if (delete_file_references.has_value()) {
		delete_file_references_tmp.emplace();
		(*delete_file_references_tmp).reserve((*delete_file_references).size());
		for (auto &item : (*delete_file_references)) {
			(*delete_file_references_tmp).emplace_back(item);
		}
	}
	if (delete_file_references_tmp.has_value()) {
		builder.SetDeleteFileReferences(std::move(delete_file_references_tmp));
	}
	unique_ptr<Expression> residual_filter_tmp;
	if (residual_filter != nullptr) {
		residual_filter_tmp = nullptr;
		residual_filter_tmp = residual_filter ? make_uniq<Expression>(residual_filter->Copy()) : nullptr;
	}
	if (residual_filter_tmp != nullptr) {
		builder.SetResidualFilter(std::move(residual_filter_tmp));
	}
	return builder.Build();
}

string FileScanTask::Validate() const {
	string error;
	error = data_file.Validate();
	if (!error.empty()) {
		return error;
	}
	if (residual_filter != nullptr) {
		error = residual_filter->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
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
