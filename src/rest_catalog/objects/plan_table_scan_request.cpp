
#include "rest_catalog/objects/plan_table_scan_request.hpp"

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

PlanTableScanRequest::PlanTableScanRequest(optional<int64_t> snapshot_id_p, optional<vector<FieldName>> select_p,
                                           unique_ptr<Expression> filter_p, optional<int64_t> min_rows_requested_p,
                                           optional<bool> case_sensitive_p, optional<bool> use_snapshot_schema_p,
                                           optional<int64_t> start_snapshot_id_p, optional<int64_t> end_snapshot_id_p,
                                           optional<vector<FieldName>> stats_fields_p)
    : snapshot_id(std::move(snapshot_id_p)), select(std::move(select_p)), filter(std::move(filter_p)),
      min_rows_requested(std::move(min_rows_requested_p)), case_sensitive(std::move(case_sensitive_p)),
      use_snapshot_schema(std::move(use_snapshot_schema_p)), start_snapshot_id(std::move(start_snapshot_id_p)),
      end_snapshot_id(std::move(end_snapshot_id_p)), stats_fields(std::move(stats_fields_p)) {
}

PlanTableScanRequestBuilder::PlanTableScanRequestBuilder() {
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetSelect(vector<FieldName> value) {
	select_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetFilter(unique_ptr<Expression> value) {
	filter_ = std::move(value);
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetMinRowsRequested(int64_t value) {
	min_rows_requested_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetCaseSensitive(bool value) {
	case_sensitive_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetUseSnapshotSchema(bool value) {
	use_snapshot_schema_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetStartSnapshotId(int64_t value) {
	start_snapshot_id_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetEndSnapshotId(int64_t value) {
	end_snapshot_id_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequestBuilder &PlanTableScanRequestBuilder::SetStatsFields(vector<FieldName> value) {
	stats_fields_.emplace(std::move(value));
	return *this;
}

PlanTableScanRequest PlanTableScanRequestBuilder::Build() {
	auto result = PlanTableScanRequest(std::move(snapshot_id_), std::move(select_), std::move(filter_),
	                                   std::move(min_rows_requested_), std::move(case_sensitive_),
	                                   std::move(use_snapshot_schema_), std::move(start_snapshot_id_),
	                                   std::move(end_snapshot_id_), std::move(stats_fields_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PlanTableScanRequestBuilder::TryBuild(optional<PlanTableScanRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string PlanTableScanRequest::TryFromJSON(yyjson_val *obj, PlanTableScanRequestBuilder &builder) {
	try {
		auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
		if (snapshot_id_val) {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
		auto select_val = yyjson_obj_get(obj, "select");
		if (select_val) {
			vector<FieldName> select;
			if (yyjson_is_arr(select_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(select_val, idx, max, val) {
					auto tmp = FieldName::FromJSON(val);
					select.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'select' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(select_val)));
			}
			builder.SetSelect(std::move(select));
		}
		auto filter_val = yyjson_obj_get(obj, "filter");
		if (filter_val) {
			unique_ptr<Expression> filter;
			filter = make_uniq<Expression>(Expression::FromJSON(filter_val));
			builder.SetFilter(std::move(filter));
		}
		auto min_rows_requested_val = yyjson_obj_get(obj, "min-rows-requested");
		if (min_rows_requested_val) {
			int64_t min_rows_requested;
			if (yyjson_is_sint(min_rows_requested_val)) {
				min_rows_requested = yyjson_get_sint(min_rows_requested_val);
			} else if (yyjson_is_uint(min_rows_requested_val)) {
				min_rows_requested = yyjson_get_uint(min_rows_requested_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'min_rows_requested' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(min_rows_requested_val)));
			}
			builder.SetMinRowsRequested(std::move(min_rows_requested));
		}
		auto case_sensitive_val = yyjson_obj_get(obj, "case-sensitive");
		if (case_sensitive_val) {
			bool case_sensitive;
			if (yyjson_is_bool(case_sensitive_val)) {
				case_sensitive = yyjson_get_bool(case_sensitive_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'case_sensitive' is not of type 'boolean', found '%s' instead",
				    yyjson_get_type_desc(case_sensitive_val)));
			}
			builder.SetCaseSensitive(std::move(case_sensitive));
		}
		auto use_snapshot_schema_val = yyjson_obj_get(obj, "use-snapshot-schema");
		if (use_snapshot_schema_val) {
			bool use_snapshot_schema;
			if (yyjson_is_bool(use_snapshot_schema_val)) {
				use_snapshot_schema = yyjson_get_bool(use_snapshot_schema_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'use_snapshot_schema' is not of type 'boolean', found '%s' instead",
				    yyjson_get_type_desc(use_snapshot_schema_val)));
			}
			builder.SetUseSnapshotSchema(std::move(use_snapshot_schema));
		}
		auto start_snapshot_id_val = yyjson_obj_get(obj, "start-snapshot-id");
		if (start_snapshot_id_val) {
			int64_t start_snapshot_id;
			if (yyjson_is_sint(start_snapshot_id_val)) {
				start_snapshot_id = yyjson_get_sint(start_snapshot_id_val);
			} else if (yyjson_is_uint(start_snapshot_id_val)) {
				start_snapshot_id = yyjson_get_uint(start_snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'start_snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(start_snapshot_id_val)));
			}
			builder.SetStartSnapshotId(std::move(start_snapshot_id));
		}
		auto end_snapshot_id_val = yyjson_obj_get(obj, "end-snapshot-id");
		if (end_snapshot_id_val) {
			int64_t end_snapshot_id;
			if (yyjson_is_sint(end_snapshot_id_val)) {
				end_snapshot_id = yyjson_get_sint(end_snapshot_id_val);
			} else if (yyjson_is_uint(end_snapshot_id_val)) {
				end_snapshot_id = yyjson_get_uint(end_snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'end_snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(end_snapshot_id_val)));
			}
			builder.SetEndSnapshotId(std::move(end_snapshot_id));
		}
		auto stats_fields_val = yyjson_obj_get(obj, "stats-fields");
		if (stats_fields_val) {
			vector<FieldName> stats_fields;
			if (yyjson_is_arr(stats_fields_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(stats_fields_val, idx, max, val) {
					auto tmp = FieldName::FromJSON(val);
					stats_fields.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "PlanTableScanRequest property 'stats_fields' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(stats_fields_val)));
			}
			builder.SetStatsFields(std::move(stats_fields));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PlanTableScanRequest PlanTableScanRequest::FromJSON(yyjson_val *obj) {
	PlanTableScanRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

PlanTableScanRequest PlanTableScanRequest::Copy() const {
	PlanTableScanRequestBuilder builder;
	optional<int64_t> snapshot_id_tmp;
	if (snapshot_id.has_value()) {
		snapshot_id_tmp.emplace();
		(*snapshot_id_tmp) = (*snapshot_id);
	}
	if (snapshot_id_tmp.has_value()) {
		builder.SetSnapshotId(std::move((*snapshot_id_tmp)));
	}
	optional<vector<FieldName>> select_tmp;
	if (select.has_value()) {
		select_tmp.emplace();
		(*select_tmp).reserve((*select).size());
		for (auto &item : (*select)) {
			(*select_tmp).emplace_back(item.Copy());
		}
	}
	if (select_tmp.has_value()) {
		builder.SetSelect(std::move((*select_tmp)));
	}
	unique_ptr<Expression> filter_tmp;
	if (filter != nullptr) {
		filter_tmp = nullptr;
		filter_tmp = filter ? make_uniq<Expression>(filter->Copy()) : nullptr;
	}
	if (filter_tmp != nullptr) {
		builder.SetFilter(std::move(filter_tmp));
	}
	optional<int64_t> min_rows_requested_tmp;
	if (min_rows_requested.has_value()) {
		min_rows_requested_tmp.emplace();
		(*min_rows_requested_tmp) = (*min_rows_requested);
	}
	if (min_rows_requested_tmp.has_value()) {
		builder.SetMinRowsRequested(std::move((*min_rows_requested_tmp)));
	}
	optional<bool> case_sensitive_tmp;
	if (case_sensitive.has_value()) {
		case_sensitive_tmp.emplace();
		(*case_sensitive_tmp) = (*case_sensitive);
	}
	if (case_sensitive_tmp.has_value()) {
		builder.SetCaseSensitive(std::move((*case_sensitive_tmp)));
	}
	optional<bool> use_snapshot_schema_tmp;
	if (use_snapshot_schema.has_value()) {
		use_snapshot_schema_tmp.emplace();
		(*use_snapshot_schema_tmp) = (*use_snapshot_schema);
	}
	if (use_snapshot_schema_tmp.has_value()) {
		builder.SetUseSnapshotSchema(std::move((*use_snapshot_schema_tmp)));
	}
	optional<int64_t> start_snapshot_id_tmp;
	if (start_snapshot_id.has_value()) {
		start_snapshot_id_tmp.emplace();
		(*start_snapshot_id_tmp) = (*start_snapshot_id);
	}
	if (start_snapshot_id_tmp.has_value()) {
		builder.SetStartSnapshotId(std::move((*start_snapshot_id_tmp)));
	}
	optional<int64_t> end_snapshot_id_tmp;
	if (end_snapshot_id.has_value()) {
		end_snapshot_id_tmp.emplace();
		(*end_snapshot_id_tmp) = (*end_snapshot_id);
	}
	if (end_snapshot_id_tmp.has_value()) {
		builder.SetEndSnapshotId(std::move((*end_snapshot_id_tmp)));
	}
	optional<vector<FieldName>> stats_fields_tmp;
	if (stats_fields.has_value()) {
		stats_fields_tmp.emplace();
		(*stats_fields_tmp).reserve((*stats_fields).size());
		for (auto &item : (*stats_fields)) {
			(*stats_fields_tmp).emplace_back(item.Copy());
		}
	}
	if (stats_fields_tmp.has_value()) {
		builder.SetStatsFields(std::move((*stats_fields_tmp)));
	}
	return builder.Build();
}

string PlanTableScanRequest::Validate() const {
	string error;
	if (select.has_value()) {
		for (const auto &item : (*select)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (filter != nullptr) {
		error = filter->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (stats_fields.has_value()) {
		for (const auto &item : (*stats_fields)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void PlanTableScanRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: snapshot-id
	if (snapshot_id.has_value()) {
		auto &snapshot_id_value = *snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id_value);
	}

	// Serialize: select
	if (select.has_value()) {
		auto &select_value = *select;
		yyjson_mut_val *select_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : select_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(select_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "select", select_value_arr);
	}

	// Serialize: filter
	if (filter != nullptr) {
		yyjson_mut_val *filter_val = filter->ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "filter", filter_val);
	}

	// Serialize: min-rows-requested
	if (min_rows_requested.has_value()) {
		auto &min_rows_requested_value = *min_rows_requested;
		yyjson_mut_obj_add_sint(doc, obj, "min-rows-requested", min_rows_requested_value);
	}

	// Serialize: case-sensitive
	if (case_sensitive.has_value()) {
		auto &case_sensitive_value = *case_sensitive;
		yyjson_mut_obj_add_bool(doc, obj, "case-sensitive", case_sensitive_value);
	}

	// Serialize: use-snapshot-schema
	if (use_snapshot_schema.has_value()) {
		auto &use_snapshot_schema_value = *use_snapshot_schema;
		yyjson_mut_obj_add_bool(doc, obj, "use-snapshot-schema", use_snapshot_schema_value);
	}

	// Serialize: start-snapshot-id
	if (start_snapshot_id.has_value()) {
		auto &start_snapshot_id_value = *start_snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "start-snapshot-id", start_snapshot_id_value);
	}

	// Serialize: end-snapshot-id
	if (end_snapshot_id.has_value()) {
		auto &end_snapshot_id_value = *end_snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "end-snapshot-id", end_snapshot_id_value);
	}

	// Serialize: stats-fields
	if (stats_fields.has_value()) {
		auto &stats_fields_value = *stats_fields;
		yyjson_mut_val *stats_fields_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : stats_fields_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(stats_fields_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "stats-fields", stats_fields_value_arr);
	}
}

yyjson_mut_val *PlanTableScanRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
