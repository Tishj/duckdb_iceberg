
#include "rest_catalog/objects/add_view_version_update.hpp"

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

AddViewVersionUpdate::AddViewVersionUpdate(BaseUpdate base_update_p, ViewVersion view_version_p)
    : base_update(std::move(base_update_p)), view_version(std::move(view_version_p)) {
}

AddViewVersionUpdateBuilder::AddViewVersionUpdateBuilder() {
}

AddViewVersionUpdateBuilder &AddViewVersionUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AddViewVersionUpdateBuilder &AddViewVersionUpdateBuilder::SetViewVersion(ViewVersion value) {
	view_version_.emplace(std::move(value));
	has_view_version_ = true;
	return *this;
}

AddViewVersionUpdate AddViewVersionUpdateBuilder::Build() {
	if (!has_view_version_) {
		throw InvalidInputException("AddViewVersionUpdate required property 'view-version' is missing");
	}
	auto result = AddViewVersionUpdate(std::move(*base_update_), std::move(*view_version_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AddViewVersionUpdateBuilder::TryBuild(optional<AddViewVersionUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddViewVersionUpdate AddViewVersionUpdate::FromJSON(yyjson_val *obj) {
	AddViewVersionUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto view_version_val = yyjson_obj_get(obj, "view-version");
	if (!view_version_val) {
		throw InvalidInputException("AddViewVersionUpdate required property 'view-version' is missing");
	} else {
		builder.SetViewVersion(ViewVersion::FromJSON(view_version_val));
	}
	return builder.Build();
}

string AddViewVersionUpdate::TryFromJSON(yyjson_val *obj, optional<AddViewVersionUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddViewVersionUpdate AddViewVersionUpdate::Copy() const {
	AddViewVersionUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	auto view_version_tmp = view_version.Copy();
	builder.SetViewVersion(std::move(view_version_tmp));
	return builder.Build();
}

string AddViewVersionUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = view_version.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void AddViewVersionUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: view-version
	yyjson_mut_val *view_version_val = view_version.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "view-version", view_version_val);
}

yyjson_mut_val *AddViewVersionUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
