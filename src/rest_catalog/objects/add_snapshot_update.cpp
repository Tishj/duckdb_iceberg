
#include "rest_catalog/objects/add_snapshot_update.hpp"

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

AddSnapshotUpdate::AddSnapshotUpdate(BaseUpdate base_update_p, Snapshot snapshot_p)
    : base_update(std::move(base_update_p)), snapshot(std::move(snapshot_p)) {
}
AddSnapshotUpdate::AddSnapshotUpdate(const AddSnapshotUpdate &other)
    : base_update(other.base_update.Copy()), snapshot(other.snapshot.Copy()) {
}
AddSnapshotUpdate::AddSnapshotUpdate(AddSnapshotUpdate &&other)
    : AddSnapshotUpdate(static_cast<const AddSnapshotUpdate &>(other)) {
}

AddSnapshotUpdateBuilder::AddSnapshotUpdateBuilder() {
}

AddSnapshotUpdateBuilder &AddSnapshotUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AddSnapshotUpdateBuilder &AddSnapshotUpdateBuilder::SetSnapshot(Snapshot value) {
	snapshot_.emplace(std::move(value));
	has_snapshot_ = true;
	return *this;
}

AddSnapshotUpdate AddSnapshotUpdateBuilder::Build() {
	if (!has_snapshot_) {
		throw InvalidInputException("AddSnapshotUpdate required property 'snapshot' is missing");
	}
	auto result = AddSnapshotUpdate(std::move(*base_update_), std::move(*snapshot_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AddSnapshotUpdateBuilder::TryBuild(optional<AddSnapshotUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AddSnapshotUpdate::TryFromJSON(yyjson_val *obj, AddSnapshotUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto snapshot_val = yyjson_obj_get(obj, "snapshot");
		if (!snapshot_val) {
			throw InvalidInputException("AddSnapshotUpdate required property 'snapshot' is missing");
		} else {
			builder.SetSnapshot(Snapshot::FromJSON(snapshot_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddSnapshotUpdate AddSnapshotUpdate::FromJSON(yyjson_val *obj) {
	AddSnapshotUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AddSnapshotUpdate AddSnapshotUpdate::Copy() const {
	return AddSnapshotUpdate(*this);
}

string AddSnapshotUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = snapshot.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void AddSnapshotUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: snapshot
	yyjson_mut_val *snapshot_val = snapshot.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "snapshot", snapshot_val);
}

yyjson_mut_val *AddSnapshotUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
