
#include "rest_catalog/objects/set_statistics_update.hpp"

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

SetStatisticsUpdate::SetStatisticsUpdate(BaseUpdate base_update_p, StatisticsFile statistics_p,
                                         optional<int64_t> snapshot_id_p)
    : base_update(std::move(base_update_p)), statistics(std::move(statistics_p)),
      snapshot_id(std::move(snapshot_id_p)) {
}
SetStatisticsUpdate::SetStatisticsUpdate(const SetStatisticsUpdate &other)
    : base_update(other.base_update.Copy()), statistics(other.statistics.Copy()),
      snapshot_id((other.snapshot_id.has_value() ? optional<int64_t>((*other.snapshot_id)) : optional<int64_t>())) {
}
SetStatisticsUpdate::SetStatisticsUpdate(SetStatisticsUpdate &&other)
    : SetStatisticsUpdate(static_cast<const SetStatisticsUpdate &>(other)) {
}

SetStatisticsUpdateBuilder::SetStatisticsUpdateBuilder() {
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetStatistics(StatisticsFile value) {
	statistics_.emplace(std::move(value));
	has_statistics_ = true;
	return *this;
}

SetStatisticsUpdateBuilder &SetStatisticsUpdateBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	return *this;
}

SetStatisticsUpdate SetStatisticsUpdateBuilder::Build() {
	if (!has_statistics_) {
		throw InvalidInputException("SetStatisticsUpdate required property 'statistics' is missing");
	}
	auto result = SetStatisticsUpdate(std::move(*base_update_), std::move(*statistics_), std::move(snapshot_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> SetStatisticsUpdateBuilder::TryBuild(optional<SetStatisticsUpdate> &result) {
	if (!has_statistics_) {
		return "SetStatisticsUpdate required property 'statistics' is missing";
	}
	auto built = SetStatisticsUpdate(std::move(*base_update_), std::move(*statistics_), std::move(snapshot_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> SetStatisticsUpdate::TryFromJSON(yyjson_val *obj, SetStatisticsUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto statistics_val = yyjson_obj_get(obj, "statistics");
		if (!statistics_val) {
			throw InvalidInputException("SetStatisticsUpdate required property 'statistics' is missing");
		} else {
			builder.SetStatistics(StatisticsFile::FromJSON(statistics_val));
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
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetStatisticsUpdate SetStatisticsUpdate::FromJSON(yyjson_val *obj) {
	SetStatisticsUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

SetStatisticsUpdate SetStatisticsUpdate::Copy() const {
	return SetStatisticsUpdate(*this);
}

optional<string> SetStatisticsUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	error = statistics.Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
