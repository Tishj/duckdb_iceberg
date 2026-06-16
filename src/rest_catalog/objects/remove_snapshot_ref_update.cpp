
#include "rest_catalog/objects/remove_snapshot_ref_update.hpp"

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

RemoveSnapshotRefUpdate::RemoveSnapshotRefUpdate(BaseUpdate base_update_p, string ref_name_p)
    : base_update(std::move(base_update_p)), ref_name(std::move(ref_name_p)) {
}

RemoveSnapshotRefUpdateBuilder::RemoveSnapshotRefUpdateBuilder() {
}

RemoveSnapshotRefUpdateBuilder &RemoveSnapshotRefUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

RemoveSnapshotRefUpdateBuilder &RemoveSnapshotRefUpdateBuilder::SetRefName(string value) {
	ref_name_.emplace(std::move(value));
	has_ref_name_ = true;
	return *this;
}

RemoveSnapshotRefUpdate RemoveSnapshotRefUpdateBuilder::Build() {
	if (!has_ref_name_) {
		throw InvalidInputException("RemoveSnapshotRefUpdate required property 'ref-name' is missing");
	}
	auto result = RemoveSnapshotRefUpdate(std::move(*base_update_), std::move(*ref_name_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemoveSnapshotRefUpdateBuilder::TryBuild(optional<RemoveSnapshotRefUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveSnapshotRefUpdate RemoveSnapshotRefUpdate::FromJSON(yyjson_val *obj) {
	RemoveSnapshotRefUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto ref_name_val = yyjson_obj_get(obj, "ref-name");
	if (!ref_name_val) {
		throw InvalidInputException("RemoveSnapshotRefUpdate required property 'ref-name' is missing");
	} else {
		string ref_name;
		if (yyjson_is_str(ref_name_val)) {
			ref_name = yyjson_get_str(ref_name_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "RemoveSnapshotRefUpdate property 'ref_name' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(ref_name_val)));
		}
		builder.SetRefName(std::move(ref_name));
	}
	return builder.Build();
}

string RemoveSnapshotRefUpdate::TryFromJSON(yyjson_val *obj, optional<RemoveSnapshotRefUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveSnapshotRefUpdate RemoveSnapshotRefUpdate::Copy() const {
	RemoveSnapshotRefUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	string ref_name_tmp;
	ref_name_tmp = ref_name;
	builder.SetRefName(std::move(ref_name_tmp));
	return builder.Build();
}

string RemoveSnapshotRefUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void RemoveSnapshotRefUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: ref-name
	yyjson_mut_obj_add_strcpy(doc, obj, "ref-name", ref_name.c_str());
}

yyjson_mut_val *RemoveSnapshotRefUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
