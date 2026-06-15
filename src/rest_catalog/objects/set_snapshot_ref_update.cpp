
#include "rest_catalog/objects/set_snapshot_ref_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SetSnapshotRefUpdate::SetSnapshotRefUpdate(BaseUpdate base_update_p, SnapshotReference snapshot_reference_p,
                                           string ref_name_p)
    : base_update(std::move(base_update_p)), snapshot_reference(std::move(snapshot_reference_p)),
      ref_name(std::move(ref_name_p)) {
}

SetSnapshotRefUpdateBuilder::SetSnapshotRefUpdateBuilder() {
}

SetSnapshotRefUpdateBuilder &SetSnapshotRefUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

SetSnapshotRefUpdateBuilder &SetSnapshotRefUpdateBuilder::SetSnapshotReference(SnapshotReference value) {
	snapshot_reference_ = std::move(value);
	return *this;
}

SetSnapshotRefUpdateBuilder &SetSnapshotRefUpdateBuilder::SetRefName(string value) {
	ref_name_ = std::move(value);
	has_ref_name_ = true;
	return *this;
}

SetSnapshotRefUpdate SetSnapshotRefUpdateBuilder::Build() {
	if (!has_ref_name_) {
		throw InvalidInputException("SetSnapshotRefUpdate required property 'ref-name' is missing");
	}
	auto result =
	    SetSnapshotRefUpdate(std::move(*base_update_), std::move(*snapshot_reference_), std::move(*ref_name_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetSnapshotRefUpdateBuilder::TryBuild(optional<SetSnapshotRefUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetSnapshotRefUpdate SetSnapshotRefUpdate::FromJSON(yyjson_val *obj) {
	SetSnapshotRefUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	builder.SetSnapshotReference(SnapshotReference::FromJSON(obj));
	auto ref_name_val = yyjson_obj_get(obj, "ref-name");
	if (!ref_name_val) {
		throw InvalidInputException("SetSnapshotRefUpdate required property 'ref-name' is missing");
	} else {
		string ref_name;
		if (yyjson_is_str(ref_name_val)) {
			ref_name = yyjson_get_str(ref_name_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SetSnapshotRefUpdate property 'ref_name' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(ref_name_val)));
		}
		builder.SetRefName(std::move(ref_name));
	}
	return builder.Build();
}

string SetSnapshotRefUpdate::TryFromJSON(yyjson_val *obj, optional<SetSnapshotRefUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetSnapshotRefUpdate SetSnapshotRefUpdate::Copy() const {
	SetSnapshotRefUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	optional<SnapshotReference> snapshot_reference_tmp;
	snapshot_reference_tmp = snapshot_reference.Copy();
	builder.SetSnapshotReference(std::move(*snapshot_reference_tmp));
	string ref_name_tmp;
	ref_name_tmp = ref_name;
	builder.SetRefName(std::move(ref_name_tmp));
	return builder.Build();
}

string SetSnapshotRefUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = snapshot_reference.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SetSnapshotRefUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize base class: SnapshotReference
	snapshot_reference.PopulateJSON(doc, obj);

	// Serialize: ref-name
	yyjson_mut_obj_add_strcpy(doc, obj, "ref-name", ref_name.c_str());
}

yyjson_mut_val *SetSnapshotRefUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
