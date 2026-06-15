
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

RemovePartitionStatisticsUpdate::RemovePartitionStatisticsUpdate(BaseUpdate base_update_p, int64_t snapshot_id_p)
    : base_update(std::move(base_update_p)), snapshot_id(std::move(snapshot_id_p)) {
}

RemovePartitionStatisticsUpdateBuilder::RemovePartitionStatisticsUpdateBuilder() {
}

RemovePartitionStatisticsUpdateBuilder &RemovePartitionStatisticsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

RemovePartitionStatisticsUpdateBuilder &RemovePartitionStatisticsUpdateBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_ = std::move(value);
	has_snapshot_id_ = true;
	return *this;
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdateBuilder::Build() {
	if (!has_snapshot_id_) {
		throw InvalidInputException("RemovePartitionStatisticsUpdate required property 'snapshot-id' is missing");
	}
	auto result = RemovePartitionStatisticsUpdate(std::move(*base_update_), std::move(*snapshot_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemovePartitionStatisticsUpdateBuilder::TryBuild(optional<RemovePartitionStatisticsUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdate::FromJSON(yyjson_val *obj) {
	RemovePartitionStatisticsUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		throw InvalidInputException("RemovePartitionStatisticsUpdate required property 'snapshot-id' is missing");
	} else {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "RemovePartitionStatisticsUpdate property 'snapshot_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	return builder.Build();
}

string RemovePartitionStatisticsUpdate::TryFromJSON(yyjson_val *obj,
                                                    optional<RemovePartitionStatisticsUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemovePartitionStatisticsUpdate RemovePartitionStatisticsUpdate::Copy() const {
	RemovePartitionStatisticsUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	return builder.Build();
}

string RemovePartitionStatisticsUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
