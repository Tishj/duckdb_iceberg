
#include "rest_catalog/objects/scan_tasks.hpp"

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

ScanTasks::ScanTasks(optional<vector<DeleteFile>> delete_files_p, optional<vector<FileScanTask>> file_scan_tasks_p,
                     optional<vector<PlanTask>> plan_tasks_p)
    : delete_files(std::move(delete_files_p)), file_scan_tasks(std::move(file_scan_tasks_p)),
      plan_tasks(std::move(plan_tasks_p)) {
}
ScanTasks::ScanTasks(const ScanTasks &other)
    : delete_files((other.delete_files.has_value() ? optional<vector<DeleteFile>>(([&]() {
	      vector<DeleteFile> copied;
	      copied.reserve((*other.delete_files).size());
	      for (const auto &item : (*other.delete_files)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                   : optional<vector<DeleteFile>>())),
      file_scan_tasks((other.file_scan_tasks.has_value() ? optional<vector<FileScanTask>>(([&]() {
	      vector<FileScanTask> copied;
	      copied.reserve((*other.file_scan_tasks).size());
	      for (const auto &item : (*other.file_scan_tasks)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                         : optional<vector<FileScanTask>>())),
      plan_tasks((other.plan_tasks.has_value() ? optional<vector<PlanTask>>(([&]() {
	      vector<PlanTask> copied;
	      copied.reserve((*other.plan_tasks).size());
	      for (const auto &item : (*other.plan_tasks)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                               : optional<vector<PlanTask>>())) {
}
ScanTasks::ScanTasks(ScanTasks &&other) : ScanTasks(static_cast<const ScanTasks &>(other)) {
}

ScanTasksBuilder::ScanTasksBuilder() {
}

ScanTasksBuilder &ScanTasksBuilder::SetDeleteFiles(vector<DeleteFile> value) {
	delete_files_.emplace(std::move(value));
	return *this;
}

ScanTasksBuilder &ScanTasksBuilder::SetFileScanTasks(vector<FileScanTask> value) {
	file_scan_tasks_.emplace(std::move(value));
	return *this;
}

ScanTasksBuilder &ScanTasksBuilder::SetPlanTasks(vector<PlanTask> value) {
	plan_tasks_.emplace(std::move(value));
	return *this;
}

ScanTasks ScanTasksBuilder::Build() {
	auto result = ScanTasks(std::move(delete_files_), std::move(file_scan_tasks_), std::move(plan_tasks_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> ScanTasksBuilder::TryBuild(optional<ScanTasks> &result) {
	auto built = ScanTasks(std::move(delete_files_), std::move(file_scan_tasks_), std::move(plan_tasks_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> ScanTasks::TryFromJSON(yyjson_val *obj, ScanTasksBuilder &builder) {
	try {
		auto delete_files_val = yyjson_obj_get(obj, "delete-files");
		if (delete_files_val) {
			vector<DeleteFile> delete_files;
			if (yyjson_is_arr(delete_files_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(delete_files_val, idx, max, val) {
					auto tmp = DeleteFile::FromJSON(val);
					delete_files.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ScanTasks property 'delete_files' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(delete_files_val)));
			}
			builder.SetDeleteFiles(std::move(delete_files));
		}
		auto file_scan_tasks_val = yyjson_obj_get(obj, "file-scan-tasks");
		if (file_scan_tasks_val) {
			vector<FileScanTask> file_scan_tasks;
			if (yyjson_is_arr(file_scan_tasks_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(file_scan_tasks_val, idx, max, val) {
					auto tmp = FileScanTask::FromJSON(val);
					file_scan_tasks.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ScanTasks property 'file_scan_tasks' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(file_scan_tasks_val)));
			}
			builder.SetFileScanTasks(std::move(file_scan_tasks));
		}
		auto plan_tasks_val = yyjson_obj_get(obj, "plan-tasks");
		if (plan_tasks_val) {
			vector<PlanTask> plan_tasks;
			if (yyjson_is_arr(plan_tasks_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(plan_tasks_val, idx, max, val) {
					auto tmp = PlanTask::FromJSON(val);
					plan_tasks.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ScanTasks property 'plan_tasks' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(plan_tasks_val)));
			}
			builder.SetPlanTasks(std::move(plan_tasks));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ScanTasks ScanTasks::FromJSON(yyjson_val *obj) {
	ScanTasksBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

ScanTasks ScanTasks::Copy() const {
	return ScanTasks(*this);
}

optional<string> ScanTasks::Validate() const {
	optional<string> error;
	if (delete_files.has_value()) {
		for (const auto &item : (*delete_files)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	if (file_scan_tasks.has_value()) {
		for (const auto &item : (*file_scan_tasks)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	if (plan_tasks.has_value()) {
		for (const auto &item : (*plan_tasks)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	return nullopt;
}

void ScanTasks::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: delete-files
	if (delete_files.has_value()) {
		auto &delete_files_value = *delete_files;
		yyjson_mut_val *delete_files_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : delete_files_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(delete_files_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "delete-files", delete_files_value_arr);
	}

	// Serialize: file-scan-tasks
	if (file_scan_tasks.has_value()) {
		auto &file_scan_tasks_value = *file_scan_tasks;
		yyjson_mut_val *file_scan_tasks_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : file_scan_tasks_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(file_scan_tasks_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "file-scan-tasks", file_scan_tasks_value_arr);
	}

	// Serialize: plan-tasks
	if (plan_tasks.has_value()) {
		auto &plan_tasks_value = *plan_tasks;
		yyjson_mut_val *plan_tasks_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : plan_tasks_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(plan_tasks_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "plan-tasks", plan_tasks_value_arr);
	}
}

yyjson_mut_val *ScanTasks::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
