
#include "rest_catalog/objects/assert_ref_snapshot_id.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssertRefSnapshotId::AssertRefSnapshotId() {
}

AssertRefSnapshotIdBuilder::AssertRefSnapshotIdBuilder() {
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetType(TableRequirementType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetRef(string value) {
	result_.ref = std::move(value);
	has_ref_ = true;
	return *this;
}

AssertRefSnapshotIdBuilder &AssertRefSnapshotIdBuilder::SetSnapshotId(int64_t value) {
	result_.snapshot_id = std::move(value);
	return *this;
}

string AssertRefSnapshotIdBuilder::TryBuild(AssertRefSnapshotId &result) {
	if (!has_type_) {
		return "AssertRefSnapshotId required property 'type' is missing";
	}
	if (!has_ref_) {
		return "AssertRefSnapshotId required property 'ref' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssertRefSnapshotId AssertRefSnapshotIdBuilder::Build() {
	AssertRefSnapshotId result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssertRefSnapshotId AssertRefSnapshotId::FromJSON(yyjson_val *obj) {
	AssertRefSnapshotId res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssertRefSnapshotId AssertRefSnapshotId::Copy() const {
	AssertRefSnapshotId res;
	res.type = type.Copy();
	res.ref = ref;
	if (snapshot_id.has_value()) {
		res.snapshot_id.emplace();
		(*res.snapshot_id) = (*snapshot_id);
	}
	return res;
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

string AssertRefSnapshotId::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AssertRefSnapshotId required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto ref_val = yyjson_obj_get(obj, "ref");
	if (!ref_val) {
		return "AssertRefSnapshotId required property 'ref' is missing";
	} else {
		if (yyjson_is_str(ref_val)) {
			ref = yyjson_get_str(ref_val);
		} else {
			return StringUtil::Format("AssertRefSnapshotId property 'ref' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(ref_val));
		}
	}
	auto snapshot_id_val = yyjson_obj_get(obj, "snapshot-id");
	if (snapshot_id_val) {
		if (yyjson_is_null(snapshot_id_val)) {
			//! do nothing, property is explicitly nullable
		} else {
			int64_t snapshot_id_tmp;
			if (yyjson_is_sint(snapshot_id_val)) {
				snapshot_id_tmp = yyjson_get_sint(snapshot_id_val);
			} else if (yyjson_is_uint(snapshot_id_val)) {
				snapshot_id_tmp = yyjson_get_uint(snapshot_id_val);
			} else {
				return StringUtil::Format(
				    "AssertRefSnapshotId property 'snapshot_id_tmp' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(snapshot_id_val));
			}
			snapshot_id = std::move(snapshot_id_tmp);
		}
	}
	return Validate();
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
