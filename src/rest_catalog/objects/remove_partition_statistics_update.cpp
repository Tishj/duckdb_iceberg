
#include "rest_catalog/objects/remove_partition_statistics_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RemovePartitionStatisticsUpdate::RemovePartitionStatisticsUpdate()
    : base_update(GeneratedObjectAccess::Create<BaseUpdate>()) {
}

RemovePartitionStatisticsUpdateBuilder::RemovePartitionStatisticsUpdateBuilder() {
}

RemovePartitionStatisticsUpdateBuilder &RemovePartitionStatisticsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

RemovePartitionStatisticsUpdateBuilder &RemovePartitionStatisticsUpdateBuilder::SetSnapshotId(int64_t value) {
	result_.snapshot_id = std::move(value);
	has_snapshot_id_ = true;
	return *this;
}

string RemovePartitionStatisticsUpdateBuilder::TryBuild(RemovePartitionStatisticsUpdate &result) {
	if (!has_snapshot_id_) {
		return "RemovePartitionStatisticsUpdate required property 'snapshot-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdateBuilder::Build() {
	RemovePartitionStatisticsUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdate::FromJSON(yyjson_val *obj) {
	RemovePartitionStatisticsUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdate::Copy() const {
	RemovePartitionStatisticsUpdate res;
	res.base_update = base_update.Copy();
	res.snapshot_id = snapshot_id;
	return res;
}

string RemovePartitionStatisticsUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string RemovePartitionStatisticsUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		return "RemovePartitionStatisticsUpdate required property 'snapshot-id' is missing";
	} else {
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			return StringUtil::Format(
			    "RemovePartitionStatisticsUpdate property 'snapshot_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(snapshot_id_val));
		}
	}
	return Validate();
}

void RemovePartitionStatisticsUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);
}

yyjson_mut_val *RemovePartitionStatisticsUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
