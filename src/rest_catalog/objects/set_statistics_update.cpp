
#include "rest_catalog/objects/set_statistics_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SetStatisticsUpdate::SetStatisticsUpdate(BaseUpdate base_update_p, StatisticsFile statistics_p,
                                         optional<int64_t> snapshot_id_p)
    : base_update(std::move(base_update_p)), statistics(std::move(statistics_p)),
      snapshot_id(std::move(snapshot_id_p)) {
}

SetStatisticsUpdateBuilder::SetStatisticsUpdateBuilder() {
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetStatistics(StatisticsFile value) {
	statistics_ = std::move(value);
	has_statistics_ = true;
	return *this;
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_ = std::move(value);
	return *this;
}

SetStatisticsUpdate SetStatisticsUpdateBuilder::Build() {
	if (!has_statistics_) {
		throw InvalidInputException("SetStatisticsUpdate required property 'statistics' is missing");
	}
	auto result = SetStatisticsUpdate(std::move(*base_update_), std::move(*statistics_), std::move(snapshot_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetStatisticsUpdateBuilder::TryBuild(optional<SetStatisticsUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetStatisticsUpdate SetStatisticsUpdate::FromJSON(yyjson_val *obj) {
	SetStatisticsUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto statistics_val = yyjson_obj_get(obj, "statistics");
	if (!statistics_val) {
		throw InvalidInputException("SetStatisticsUpdate required property 'statistics' is missing");
	} else {
		optional<StatisticsFile> statistics;
		statistics = StatisticsFile::FromJSON(statistics_val);
		builder.SetStatistics(std::move(*statistics));
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (snapshot_id_val) {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SetStatisticsUpdate property 'snapshot_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	return builder.Build();
}

string SetStatisticsUpdate::TryFromJSON(yyjson_val *obj, optional<SetStatisticsUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetStatisticsUpdate SetStatisticsUpdate::Copy() const {
	SetStatisticsUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	optional<StatisticsFile> statistics_tmp;
	statistics_tmp = statistics.Copy();
	builder.SetStatistics(std::move(*statistics_tmp));
	int64_t snapshot_id_tmp;
	if (snapshot_id.has_value()) {
		snapshot_id_tmp.emplace();
		(*snapshot_id_tmp) = (*snapshot_id);
	}
	if (snapshot_id_tmp.has_value()) {
		builder.SetSnapshotId(std::move(snapshot_id_tmp));
	}
	return builder.Build();
}

string SetStatisticsUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = statistics.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SetStatisticsUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: statistics
	yyjson_mut_val *statistics_val = statistics.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "statistics", statistics_val);

	// Serialize: snapshot-id
	if (snapshot_id.has_value()) {
		auto &snapshot_id_value = *snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id_value);
	}
}

yyjson_mut_val *SetStatisticsUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
