
#include "rest_catalog/objects/set_partition_statistics_update.hpp"

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

SetPartitionStatisticsUpdate::SetPartitionStatisticsUpdate(BaseUpdate base_update_p,
                                                           PartitionStatisticsFile partition_statistics_p)
    : base_update(std::move(base_update_p)), partition_statistics(std::move(partition_statistics_p)) {
}

SetPartitionStatisticsUpdateBuilder::SetPartitionStatisticsUpdateBuilder() {
}

SetPartitionStatisticsUpdateBuilder &SetPartitionStatisticsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetPartitionStatisticsUpdateBuilder &
SetPartitionStatisticsUpdateBuilder::SetPartitionStatistics(PartitionStatisticsFile value) {
	partition_statistics_.emplace(std::move(value));
	has_partition_statistics_ = true;
	return *this;
}

SetPartitionStatisticsUpdate SetPartitionStatisticsUpdateBuilder::Build() {
	if (!has_partition_statistics_) {
		throw InvalidInputException("SetPartitionStatisticsUpdate required property 'partition-statistics' is missing");
	}
	auto result = SetPartitionStatisticsUpdate(std::move(*base_update_), std::move(*partition_statistics_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetPartitionStatisticsUpdateBuilder::TryBuild(optional<SetPartitionStatisticsUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetPartitionStatisticsUpdate SetPartitionStatisticsUpdate::FromJSON(yyjson_val *obj) {
	SetPartitionStatisticsUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto partition_statistics_val = yyjson_obj_get(obj, "partition-statistics");
	if (!partition_statistics_val) {
		throw InvalidInputException("SetPartitionStatisticsUpdate required property 'partition-statistics' is missing");
	} else {
		builder.SetPartitionStatistics(PartitionStatisticsFile::FromJSON(partition_statistics_val));
	}
	return builder.Build();
}

string SetPartitionStatisticsUpdate::TryFromJSON(yyjson_val *obj, optional<SetPartitionStatisticsUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetPartitionStatisticsUpdate SetPartitionStatisticsUpdate::Copy() const {
	SetPartitionStatisticsUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	auto partition_statistics_tmp = partition_statistics.Copy();
	builder.SetPartitionStatistics(std::move(partition_statistics_tmp));
	return builder.Build();
}

string SetPartitionStatisticsUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = partition_statistics.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SetPartitionStatisticsUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: partition-statistics
	yyjson_mut_val *partition_statistics_val = partition_statistics.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "partition-statistics", partition_statistics_val);
}

yyjson_mut_val *SetPartitionStatisticsUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
