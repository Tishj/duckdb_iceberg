
#include "rest_catalog/objects/assert_ref_snapshot_id.hpp"

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

AssertRefSnapshotId::AssertRefSnapshotId(TableRequirementType type_p, string ref_p, optional<int64_t> snapshot_id_p)
    : type(std::move(type_p)), ref(std::move(ref_p)), snapshot_id(std::move(snapshot_id_p)) {
}

AssertRefSnapshotIdBuilder::AssertRefSnapshotIdBuilder() {
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetRef(string value) {
	ref_.emplace(std::move(value));
	has_ref_ = true;
	return *this;
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetSnapshotId(int64_t value) {
	snapshot_id_.emplace(std::move(value));
	return *this;
}

AssertRefSnapshotId AssertRefSnapshotIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertRefSnapshotId required property 'type' is missing");
	}
	if (!has_ref_) {
		throw InvalidInputException("AssertRefSnapshotId required property 'ref' is missing");
	}
	auto result = AssertRefSnapshotId(std::move(*type_), std::move(*ref_), std::move(snapshot_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertRefSnapshotIdBuilder::TryBuild(optional<AssertRefSnapshotId> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertRefSnapshotId AssertRefSnapshotId::FromJSON(yyjson_val *obj) {
	AssertRefSnapshotIdBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("AssertRefSnapshotId required property 'type' is missing");
	} else {
		builder.SetType(TableRequirementType::FromJSON(type_val));
	}
	auto ref_val = yyjson_obj_get(obj, "ref");
	if (!ref_val) {
		throw InvalidInputException("AssertRefSnapshotId required property 'ref' is missing");
	} else {
		string ref;
		if (yyjson_is_str(ref_val)) {
			ref = yyjson_get_str(ref_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("AssertRefSnapshotId property 'ref' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(ref_val)));
		}
		builder.SetRef(std::move(ref));
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (snapshot_id_val) {
		if (yyjson_is_null(snapshot_id_val)) {
			//! do nothing, property is explicitly nullable
		} else {
			int64_t snapshot_id;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id = yyjson_get_uint(snapshot_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "AssertRefSnapshotId property 'snapshot_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val)));
			}
			builder.SetSnapshotId(std::move(snapshot_id));
		}
	}
	return builder.Build();
}

string AssertRefSnapshotId::TryFromJSON(yyjson_val *obj, optional<AssertRefSnapshotId> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertRefSnapshotId AssertRefSnapshotId::Copy() const {
	AssertRefSnapshotIdBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	string ref_tmp;
	ref_tmp = ref;
	builder.SetRef(std::move(ref_tmp));
	optional<int64_t> snapshot_id_tmp;
	if (snapshot_id.has_value()) {
		snapshot_id_tmp.emplace();
		(*snapshot_id_tmp) = (*snapshot_id);
	}
	if (snapshot_id_tmp.has_value()) {
		builder.SetSnapshotId(std::move((*snapshot_id_tmp)));
	}
	return builder.Build();
}

string AssertRefSnapshotId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-ref-snapshot-id")) {
		return StringUtil::Format("AssertRefSnapshotId property 'type' must be assert-ref-snapshot-id, not %s",
		                          type.value);
	}
	return "";
}

void AssertRefSnapshotId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: ref
	yyjson_mut_obj_add_strcpy(doc, obj, "ref", ref.c_str());

	// Serialize: snapshot-id
	if (snapshot_id.has_value()) {
		auto &snapshot_id_value = *snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "snapshot-id", snapshot_id_value);
	}
}

yyjson_mut_val *AssertRefSnapshotId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
