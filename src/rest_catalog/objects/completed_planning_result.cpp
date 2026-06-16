
#include "rest_catalog/objects/completed_planning_result.hpp"

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

CompletedPlanningResult::CompletedPlanningResult(ScanTasks scan_tasks_p, Object5 object_5_p)
    : scan_tasks(std::move(scan_tasks_p)), object_5(std::move(object_5_p)) {
}
CompletedPlanningResult::CompletedPlanningResult(const CompletedPlanningResult &other)
    : scan_tasks(other.scan_tasks.Copy()), object_5(other.object_5.Copy()) {
}
CompletedPlanningResult::CompletedPlanningResult(CompletedPlanningResult &&other)
    : CompletedPlanningResult(static_cast<const CompletedPlanningResult &>(other)) {
}
CompletedPlanningResult::Object5::Object5(PlanStatus status_p,
                                          optional<vector<StorageCredential>> storage_credentials_p)
    : status(std::move(status_p)), storage_credentials(std::move(storage_credentials_p)) {
}
CompletedPlanningResult::Object5::Object5(const CompletedPlanningResult::Object5 &other)
    : status(other.status.Copy()),
      storage_credentials((other.storage_credentials.has_value() ? optional<vector<StorageCredential>>(([&]() {
	      vector<StorageCredential> copied;
	      copied.reserve((*other.storage_credentials).size());
	      for (const auto &item : (*other.storage_credentials)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                                 : optional<vector<StorageCredential>>())) {
}
CompletedPlanningResult::Object5::Object5(CompletedPlanningResult::Object5 &&other)
    : Object5(static_cast<const CompletedPlanningResult::Object5 &>(other)) {
}

CompletedPlanningResult::Object5Builder::Object5Builder() {
}

CompletedPlanningResult::Object5Builder &CompletedPlanningResult::Object5Builder::SetStatus(PlanStatus value) {
	status_.emplace(std::move(value));
	has_status_ = true;
	return *this;
}

CompletedPlanningResult::Object5Builder &
CompletedPlanningResult::Object5Builder::SetStorageCredentials(vector<StorageCredential> value) {
	storage_credentials_.emplace(std::move(value));
	return *this;
}

CompletedPlanningResult::Object5 CompletedPlanningResult::Object5Builder::Build() {
	if (!has_status_) {
		throw InvalidInputException("Object5 required property 'status' is missing");
	}
	auto result = CompletedPlanningResult::Object5(std::move(*status_), std::move(storage_credentials_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> CompletedPlanningResult::Object5Builder::TryBuild(optional<CompletedPlanningResult::Object5> &result) {
	if (!has_status_) {
		return "Object5 required property 'status' is missing";
	}
	auto built = CompletedPlanningResult::Object5(std::move(*status_), std::move(storage_credentials_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> CompletedPlanningResult::Object5::TryFromJSON(yyjson_val *obj, Object5Builder &builder) {
	try {
		auto status_val = yyjson_obj_get(obj, "status");
		if (!status_val) {
			throw InvalidInputException("Object5 required property 'status' is missing");
		} else {
			builder.SetStatus(PlanStatus::FromJSON(status_val));
		}
		auto storage_credentials_val = yyjson_obj_get(obj, "storage-credentials");
		if (storage_credentials_val) {
			vector<StorageCredential> storage_credentials;
			if (yyjson_is_arr(storage_credentials_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(storage_credentials_val, idx, max, val) {
					auto tmp = StorageCredential::FromJSON(val);
					storage_credentials.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "Object5 property 'storage_credentials' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(storage_credentials_val)));
			}
			builder.SetStorageCredentials(std::move(storage_credentials));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CompletedPlanningResult::Object5 CompletedPlanningResult::Object5::FromJSON(yyjson_val *obj) {
	Object5Builder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

CompletedPlanningResult::Object5 CompletedPlanningResult::Object5::Copy() const {
	return CompletedPlanningResult::Object5(*this);
}

optional<string> CompletedPlanningResult::Object5::Validate() const {
	optional<string> error;
	error = status.Validate();
	if (error) {
		return error;
	}
	if (!StringUtil::CIEquals(status.value, "completed")) {
		return StringUtil::Format("Object5 property 'status' must be completed, not %s", status.value);
	}
	if (storage_credentials.has_value()) {
		for (const auto &item : (*storage_credentials)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	return nullopt;
}

void CompletedPlanningResult::Object5::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: status
	yyjson_mut_val *status_val = status.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "status", status_val);

	// Serialize: storage-credentials
	if (storage_credentials.has_value()) {
		auto &storage_credentials_value = *storage_credentials;
		yyjson_mut_val *storage_credentials_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : storage_credentials_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(storage_credentials_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "storage-credentials", storage_credentials_value_arr);
	}
}

yyjson_mut_val *CompletedPlanningResult::Object5::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

CompletedPlanningResultBuilder::CompletedPlanningResultBuilder() {
}

CompletedPlanningResultBuilder &CompletedPlanningResultBuilder::SetScanTasks(ScanTasks value) {
	scan_tasks_.emplace(std::move(value));
	return *this;
}

CompletedPlanningResultBuilder &CompletedPlanningResultBuilder::SetObject5(CompletedPlanningResult::Object5 value) {
	object_5_.emplace(std::move(value));
	return *this;
}

CompletedPlanningResult CompletedPlanningResultBuilder::Build() {
	auto result = CompletedPlanningResult(std::move(*scan_tasks_), std::move(*object_5_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> CompletedPlanningResultBuilder::TryBuild(optional<CompletedPlanningResult> &result) {
	auto built = CompletedPlanningResult(std::move(*scan_tasks_), std::move(*object_5_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> CompletedPlanningResult::TryFromJSON(yyjson_val *obj, CompletedPlanningResultBuilder &builder) {
	try {
		builder.SetScanTasks(ScanTasks::FromJSON(obj));
		builder.SetObject5(Object5::FromJSON(obj));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CompletedPlanningResult CompletedPlanningResult::FromJSON(yyjson_val *obj) {
	CompletedPlanningResultBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

CompletedPlanningResult CompletedPlanningResult::Copy() const {
	return CompletedPlanningResult(*this);
}

optional<string> CompletedPlanningResult::Validate() const {
	optional<string> error;
	error = scan_tasks.Validate();
	if (error) {
		return error;
	}
	error = object_5.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void CompletedPlanningResult::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: ScanTasks
	scan_tasks.PopulateJSON(doc, obj);

	// Serialize base class: Object5
	object_5.PopulateJSON(doc, obj);
}

yyjson_mut_val *CompletedPlanningResult::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
