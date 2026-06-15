
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

CommitReport::CommitReport() {
}

CommitReportBuilder::CommitReportBuilder() {
}

CommitReportBuilder &CommitReportBuilder::SetTableName(string value) {
	result_.table_name = std::move(value);
	has_table_name_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetSnapshotId(int64_t value) {
	result_.snapshot_id = std::move(value);
	has_snapshot_id_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetSequenceNumber(int64_t value) {
	result_.sequence_number = std::move(value);
	has_sequence_number_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetOperation(string value) {
	result_.operation = std::move(value);
	has_operation_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetMetrics(Metrics value) {
	result_.metrics = std::move(value);
	has_metrics_ = true;
	return *this;
}

CommitReportBuilder &CommitReportBuilder::SetMetadata(case_insensitive_map_t<string> value) {
	result_.metadata = std::move(value);
	return *this;
}

string CommitReportBuilder::TryBuild(CommitReport &result) {
	if (!has_table_name_) {
		return "CommitReport required property 'table-name' is missing";
	}
	if (!has_snapshot_id_) {
		return "CommitReport required property 'snapshot-id' is missing";
	}
	if (!has_sequence_number_) {
		return "CommitReport required property 'sequence-number' is missing";
	}
	if (!has_operation_) {
		return "CommitReport required property 'operation' is missing";
	}
	if (!has_metrics_) {
		return "CommitReport required property 'metrics' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

CommitReport CommitReportBuilder::Build() {
	CommitReport result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

CommitReport CommitReport::FromJSON(yyjson_val *obj) {
	CommitReport res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

CommitReport CommitReport::Copy() const {
	CommitReport res;
	res.table_name = table_name;
	res.snapshot_id = snapshot_id;
	res.sequence_number = sequence_number;
	res.operation = operation;
	res.metrics = metrics.Copy();
	if (metadata.has_value()) {
		res.metadata.emplace();
		for (auto &entry : (*metadata)) {
			(*res.metadata).emplace(entry.first, entry.second);
		}
	}
	return res;
}

string CommitReport::Validate() const {
	string error;
	error = metrics.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string CommitReport::TryFromJSON(yyjson_val *obj) {
	string error;
	auto table_name_val = yyjson_obj_get(obj, "table-name");
	if (!table_name_val) {
		return "CommitReport required property 'table-name' is missing";
	} else {
		if (yyjson_is_str(table_name_val)) {
			table_name = yyjson_get_str(table_name_val);
		} else {
			return StringUtil::Format("CommitReport property 'table_name' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(table_name_val));
		}
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		return "CommitReport required property 'snapshot-id' is missing";
	} else {
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			return StringUtil::Format(
			    "CommitReport property 'snapshot_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(snapshot_id_val));
		}
	}
	auto sequence_number_val = yyjson_obj_get(obj, "sequence-number");
	if (!sequence_number_val) {
		return "CommitReport required property 'sequence-number' is missing";
	} else {
		if (yyjson_is_sint(sequence_number_val)) {
			sequence_number = yyjson_get_sint(sequence_number_val);
		} else if (yyjson_is_uint(sequence_number_val)) {
			sequence_number = yyjson_get_uint(sequence_number_val);
		} else {
			return StringUtil::Format(
			    "CommitReport property 'sequence_number' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(sequence_number_val));
		}
	}
	auto operation_val = yyjson_obj_get(obj, "operation");
	if (!operation_val) {
		return "CommitReport required property 'operation' is missing";
	} else {
		if (yyjson_is_str(operation_val)) {
			operation = yyjson_get_str(operation_val);
		} else {
			return StringUtil::Format("CommitReport property 'operation' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(operation_val));
		}
	}
	auto metrics_val = yyjson_obj_get(obj, "metrics");
	if (!metrics_val) {
		return "CommitReport required property 'metrics' is missing";
	} else {
		error = metrics.TryFromJSON(metrics_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto metadata_val = yyjson_obj_get(obj, "metadata");
	if (metadata_val) {
		case_insensitive_map_t<string> metadata_tmp;
		if (yyjson_is_obj(metadata_val)) {
			size_t idx, max;
			yyjson_val *key, *val;
			yyjson_obj_foreach(metadata_val, idx, max, key, val) {
				auto key_str = yyjson_get_str(key);
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					return StringUtil::Format("CommitReport property 'tmp' is not of type 'string', found '%s' instead",
					                          yyjson_get_type_desc(val));
				}
				metadata_tmp.emplace(key_str, std::move(tmp));
			}
		} else {
			return "CommitReport property 'metadata_tmp' is not of type 'object'";
		}
		metadata = std::move(metadata_tmp);
	}
	return Validate();
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
