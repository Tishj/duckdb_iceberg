
#include "rest_catalog/objects/snapshot_reference.hpp"

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

SnapshotReference::SnapshotReference(string type_p, int64_t snapshot_id_p, optional<int64_t> max_ref_age_ms_p,
                                     optional<int64_t> max_snapshot_age_ms_p, optional<int32_t> min_snapshots_to_keep_p)
    : type(std::move(type_p)), snapshot_id(std::move(snapshot_id_p)), max_ref_age_ms(std::move(max_ref_age_ms_p)),
      max_snapshot_age_ms(std::move(max_snapshot_age_ms_p)), min_snapshots_to_keep(std::move(min_snapshots_to_keep_p)) {
}

SnapshotReferenceBuilder::SnapshotReferenceBuilder() {
}

SnapshotReferenceBuilder &SnapshotReferenceBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

SnapshotReferenceBuilder &SnapshotReferenceBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	has_snapshot_id_ = true;
	return *this;
}

SnapshotReferenceBuilder &SnapshotReferenceBuilder::SetMaxRefAgeMs(int64_t value) {
	max_ref_age_ms_.emplace(std::move(value));
	return *this;
}

SnapshotReferenceBuilder &SnapshotReferenceBuilder::SetMaxSnapshotAgeMs(int64_t value) {
	max_snapshot_age_ms_.emplace(std::move(value));
	return *this;
}

SnapshotReferenceBuilder &SnapshotReferenceBuilder::SetMinSnapshotsToKeep(int32_t value) {
	min_snapshots_to_keep_.emplace(std::move(value));
	return *this;
}

SnapshotReference SnapshotReferenceBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("SnapshotReference required property 'type' is missing");
	}
	if (!has_snapshot_id_) {
		throw InvalidInputException("SnapshotReference required property 'snapshot-id' is missing");
	}
	auto result = SnapshotReference(std::move(*type_), std::move(*snapshot_id_), std::move(max_ref_age_ms_),
	                                std::move(max_snapshot_age_ms_), std::move(min_snapshots_to_keep_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SnapshotReferenceBuilder::TryBuild(optional<SnapshotReference> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotReference SnapshotReference::FromJSON(yyjson_val *obj) {
	SnapshotReferenceBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("SnapshotReference required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SnapshotReference property 'type' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (!snapshot_id_val) {
		throw InvalidInputException("SnapshotReference required property 'snapshot-id' is missing");
	} else {
		int64_t snapshot_id;
		if (yyjson_is_sint(snapshot_id_val)) {
			snapshot_id = yyjson_get_sint(snapshot_id_val);
		} else if (yyjson_is_uint(snapshot_id_val)) {
			snapshot_id = yyjson_get_uint(snapshot_id_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SnapshotReference property 'snapshot_id' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(snapshot_id_val)));
		}
		builder.SetSnapshotId(std::move(snapshot_id));
	}
	auto max_ref_age_ms_val = yyjson_obj_get(obj, "max-ref-age-ms");
	if (max_ref_age_ms_val) {
		int64_t max_ref_age_ms;
		if (yyjson_is_sint(max_ref_age_ms_val)) {
			max_ref_age_ms = yyjson_get_sint(max_ref_age_ms_val);
		} else if (yyjson_is_uint(max_ref_age_ms_val)) {
			max_ref_age_ms = yyjson_get_uint(max_ref_age_ms_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SnapshotReference property 'max_ref_age_ms' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(max_ref_age_ms_val)));
		}
		builder.SetMaxRefAgeMs(std::move(max_ref_age_ms));
	}
	auto max_snapshot_age_ms_val = yyjson_obj_get(obj, "max-snapshot-age-ms");
	if (max_snapshot_age_ms_val) {
		int64_t max_snapshot_age_ms;
		if (yyjson_is_sint(max_snapshot_age_ms_val)) {
			max_snapshot_age_ms = yyjson_get_sint(max_snapshot_age_ms_val);
		} else if (yyjson_is_uint(max_snapshot_age_ms_val)) {
			max_snapshot_age_ms = yyjson_get_uint(max_snapshot_age_ms_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SnapshotReference property 'max_snapshot_age_ms' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(max_snapshot_age_ms_val)));
		}
		builder.SetMaxSnapshotAgeMs(std::move(max_snapshot_age_ms));
	}
	auto min_snapshots_to_keep_val = yyjson_obj_get(obj, "min-snapshots-to-keep");
	if (min_snapshots_to_keep_val) {
		int32_t min_snapshots_to_keep;
		if (yyjson_is_int(min_snapshots_to_keep_val)) {
			min_snapshots_to_keep = yyjson_get_int(min_snapshots_to_keep_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SnapshotReference property 'min_snapshots_to_keep' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(min_snapshots_to_keep_val)));
		}
		builder.SetMinSnapshotsToKeep(std::move(min_snapshots_to_keep));
	}
	return builder.Build();
}

string SnapshotReference::TryFromJSON(yyjson_val *obj, optional<SnapshotReference> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotReference SnapshotReference::Copy() const {
	SnapshotReferenceBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	int64_t snapshot_id_tmp;
	snapshot_id_tmp = snapshot_id;
	builder.SetSnapshotId(std::move(snapshot_id_tmp));
	optional<int64_t> max_ref_age_ms_tmp;
	if (max_ref_age_ms.has_value()) {
		max_ref_age_ms_tmp.emplace();
		(*max_ref_age_ms_tmp) = (*max_ref_age_ms);
	}
	if (max_ref_age_ms_tmp.has_value()) {
		builder.SetMaxRefAgeMs(std::move((*max_ref_age_ms_tmp)));
	}
	optional<int64_t> max_snapshot_age_ms_tmp;
	if (max_snapshot_age_ms.has_value()) {
		max_snapshot_age_ms_tmp.emplace();
		(*max_snapshot_age_ms_tmp) = (*max_snapshot_age_ms);
	}
	if (max_snapshot_age_ms_tmp.has_value()) {
		builder.SetMaxSnapshotAgeMs(std::move((*max_snapshot_age_ms_tmp)));
	}
	optional<int32_t> min_snapshots_to_keep_tmp;
	if (min_snapshots_to_keep.has_value()) {
		min_snapshots_to_keep_tmp.emplace();
		(*min_snapshots_to_keep_tmp) = (*min_snapshots_to_keep);
	}
	if (min_snapshots_to_keep_tmp.has_value()) {
		builder.SetMinSnapshotsToKeep(std::move((*min_snapshots_to_keep_tmp)));
	}
	return builder.Build();
}

string SnapshotReference::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "tag") && !StringUtil::CIEquals(type, "branch")) {
		return StringUtil::Format("SnapshotReference property 'type' must be one of [tag, branch], not %s", type);
	}
	return "";
}

void SnapshotReference::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: snapshot-id
	yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id);

	// Serialize: max-ref-age-ms
	if (max_ref_age_ms.has_value()) {
		auto &max_ref_age_ms_value = *max_ref_age_ms;
		yyjson_mut_obj_add_sint(doc, obj, "max-ref-age-ms", max_ref_age_ms_value);
	}

	// Serialize: max-snapshot-age-ms
	if (max_snapshot_age_ms.has_value()) {
		auto &max_snapshot_age_ms_value = *max_snapshot_age_ms;
		yyjson_mut_obj_add_sint(doc, obj, "max-snapshot-age-ms", max_snapshot_age_ms_value);
	}

	// Serialize: min-snapshots-to-keep
	if (min_snapshots_to_keep.has_value()) {
		auto &min_snapshots_to_keep_value = *min_snapshots_to_keep;
		yyjson_mut_obj_add_int(doc, obj, "min-snapshots-to-keep", min_snapshots_to_keep_value);
	}
}

yyjson_mut_val *SnapshotReference::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
