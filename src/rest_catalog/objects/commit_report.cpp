
#include "rest_catalog/objects/commit_report.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

CommitReport::CommitReport(string table_name_p, int64_t snapshot_id_p, int64_t sequence_number_p, string operation_p,
                           Metrics metrics_p, optional<case_insensitive_map_t<string>> metadata_p)
    : table_name(std::move(table_name_p)), snapshot_id(std::move(snapshot_id_p)),
      sequence_number(std::move(sequence_number_p)), operation(std::move(operation_p)), metrics(std::move(metrics_p)),
      metadata(std::move(metadata_p)) {
}

CommitReportBuilder::CommitReportBuilder() {
}

CommitReportBuilder &CommitReportBuilder::SetTableName(string value) {
	table_name_ = std::move(value);
	has_table_name_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_ = std::move(value);
	has_snapshot_id_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetSequenceNumber(int64_t value) {
	sequence_number_ = std::move(value);
	has_sequence_number_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetOperation(string value) {
	operation_ = std::move(value);
	has_operation_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetMetrics(Metrics value) {
	metrics_ = std::move(value);
	has_metrics_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetMetadata(case_insensitive_map_t<string> value) {
	metadata_ = std::move(value);
	return *this;
}

CommitReport CommitReportBuilder::Build() {
	if (!has_table_name_) {
		throw InvalidInputException("CommitReport required property 'table-name' is missing");
	}
	if (!has_snapshot_id_) {
		throw InvalidInputException("CommitReport required property 'snapshot-id' is missing");
	}
	if (!has_sequence_number_) {
		throw InvalidInputException("CommitReport required property 'sequence-number' is missing");
	}
	if (!has_operation_) {
		throw InvalidInputException("CommitReport required property 'operation' is missing");
	}
	if (!has_metrics_) {
		throw InvalidInputException("CommitReport required property 'metrics' is missing");
	}
	auto result = CommitReport(std::move(*table_name_), std::move(*snapshot_id_), std::move(*sequence_number_),
	                           std::move(*operation_), std::move(*metrics_), std::move(metadata_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CommitReportBuilder::TryBuild(optional<CommitReport> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitReport CommitReport::FromJSON(yyjson_val *obj) {
	CommitReportBuilder builder;
	auto table_name_val = yyjson_obj_get(obj, "table-name");
	if (!table_name_val) {
		throw InvalidInputException("CommitReport required property 'table-name' is missing");
	} else {
		string table_name;
		if (yyjson_is_str(table_name_val)) {
			table_name = yyjson_get_str(table_name_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("CommitReport property 'table_name' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(table_name_val)));
		}
		builder.SetTableName(std::move(table_name));
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		throw InvalidInputException("CommitReport required property 'snapshot-id' is missing");
	} else {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("CommitReport property 'snapshot_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	auto sequence_number_val = yyjson_obj_get(obj, "sequence-number");
	if (!sequence_number_val) {
		throw InvalidInputException("CommitReport required property 'sequence-number' is missing");
	} else {
		int64_t sequence_number;
		if (yyjson_is_sint(sequence_number_val)) {
			sequence_number = yyjson_get_sint(sequence_number_val);
		} else if (yyjson_is_uint(sequence_number_val)) {
			sequence_number = yyjson_get_uint(sequence_number_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "CommitReport property 'sequence_number' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(sequence_number_val)));
		}
		builder.SetSequenceNumber(std::move(sequence_number));
	}
	auto operation_val = yyjson_obj_get(obj, "operation");
	if (!operation_val) {
		throw InvalidInputException("CommitReport required property 'operation' is missing");
	} else {
		string operation;
		if (yyjson_is_str(operation_val)) {
			operation = yyjson_get_str(operation_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("CommitReport property 'operation' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(operation_val)));
		}
		builder.SetOperation(std::move(operation));
	}
	auto metrics_val = yyjson_obj_get(obj, "metrics");
	if (!metrics_val) {
		throw InvalidInputException("CommitReport required property 'metrics' is missing");
	} else {
		optional<Metrics> metrics;
		metrics = Metrics::FromJSON(metrics_val);
		builder.SetMetrics(std::move(*metrics));
	}
	auto metadata_val = yyjson_obj_get(obj, "metadata");
	if (metadata_val) {
		case_insensitive_map_t<string> metadata;
		if (yyjson_is_obj(metadata_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(metadata_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("CommitReport property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				metadata.emplace(key_str, std::move(tmp));
			}
		} else {
			throw InvalidInputException("CommitReport property 'metadata' is not of type 'object'");
		}
		builder.SetMetadata(std::move(metadata));
	}
	return builder.Build();
}

string CommitReport::TryFromJSON(yyjson_val *obj, optional<CommitReport> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitReport CommitReport::Copy() const {
	CommitReportBuilder builder;
	string table_name_tmp;
	table_name_tmp = table_name;
	builder.SetTableName(std::move(table_name_tmp));
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	int64_t sequence_number_tmp;
	sequence_number_tmp = sequence_number;
	builder.SetSequenceNumber(std::move(sequence_number_tmp));
	string operation_tmp;
	operation_tmp = operation;
	builder.SetOperation(std::move(operation_tmp));
	optional<Metrics> metrics_tmp;
	metrics_tmp = metrics.Copy();
	builder.SetMetrics(std::move(*metrics_tmp));
	case_insensitive_map_t<string> metadata_tmp;
	if (metadata.has_value()) {
		metadata_tmp.emplace();
		for (auto &entry : (*metadata)) {
			(*metadata_tmp).emplace(entry.first, entry.second);
		}
	}
	if (metadata_tmp.has_value()) {
		builder.SetMetadata(std::move(metadata_tmp));
	}
	return builder.Build();
}

string CommitReport::Validate() const {
	string error;
	error = metrics.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void CommitReport::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: table-name
	yyjson_mut_obj_add_strcpy(doc, obj, "table-name", table_name.c_str());

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: sequence-number
	yyjson_mut_obj_add_sint(doc, obj, "sequence-number", sequence_number);

	// Serialize: operation
	yyjson_mut_obj_add_strcpy(doc, obj, "operation", operation.c_str());

	// Serialize: metrics
	yyjson_mut_val *metrics_val = metrics.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "metrics", metrics_val);

	// Serialize: metadata
	if (metadata.has_value()) {
		auto &metadata_value = *metadata;
		yyjson_mut_val *metadata_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : metadata_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, metadata_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "metadata", metadata_value_obj);
	}
}

yyjson_mut_val *CommitReport::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
