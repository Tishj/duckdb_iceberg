
#include "rest_catalog/objects/set_current_view_version_update.hpp"

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

SetCurrentViewVersionUpdate::SetCurrentViewVersionUpdate(BaseUpdate base_update_p, int32_t view_version_id_p)
    : base_update(std::move(base_update_p)), view_version_id(std::move(view_version_id_p)) {
}
SetCurrentViewVersionUpdate::SetCurrentViewVersionUpdate(const SetCurrentViewVersionUpdate &other)
    : base_update(other.base_update.Copy()), view_version_id(other.view_version_id) {
}
SetCurrentViewVersionUpdate::SetCurrentViewVersionUpdate(SetCurrentViewVersionUpdate &&other)
    : SetCurrentViewVersionUpdate(static_cast<const SetCurrentViewVersionUpdate &>(other)) {
}

SetCurrentViewVersionUpdateBuilder::SetCurrentViewVersionUpdateBuilder() {
}

SetCurrentViewVersionUpdateBuilder &SetCurrentViewVersionUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetCurrentViewVersionUpdateBuilder &SetCurrentViewVersionUpdateBuilder::SetViewVersionId(int32_t value) {
	view_version_id_.emplace(std::move(value));
	has_view_version_id_ = true;
	return *this;
}

SetCurrentViewVersionUpdate SetCurrentViewVersionUpdateBuilder::Build() {
	if (!has_view_version_id_) {
		throw InvalidInputException("SetCurrentViewVersionUpdate required property 'view-version-id' is missing");
	}
	auto result = SetCurrentViewVersionUpdate(std::move(*base_update_), std::move(*view_version_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SetCurrentViewVersionUpdateBuilder::TryBuild(optional<SetCurrentViewVersionUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string SetCurrentViewVersionUpdate::TryFromJSON(yyjson_val *obj, SetCurrentViewVersionUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto view_version_id_val = yyjson_obj_get(obj, "view-version-id");
		if (!view_version_id_val) {
			throw InvalidInputException("SetCurrentViewVersionUpdate required property 'view-version-id' is missing");
		} else {
			int32_t view_version_id;
			if (yyjson_is_int(view_version_id_val)) {
				view_version_id = yyjson_get_int(view_version_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format("SetCurrentViewVersionUpdate property 'view_version_id' "
				                                               "is not of type 'integer', found '%s' instead",
				                                               yyjson_get_type_desc(view_version_id_val)));
			}
			builder.SetViewVersionId(std::move(view_version_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetCurrentViewVersionUpdate SetCurrentViewVersionUpdate::FromJSON(yyjson_val *obj) {
	SetCurrentViewVersionUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

SetCurrentViewVersionUpdate SetCurrentViewVersionUpdate::Copy() const {
	return SetCurrentViewVersionUpdate(*this);
}

string SetCurrentViewVersionUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SetCurrentViewVersionUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: view-version-id
	yyjson_mut_obj_add_int(doc, obj, "view-version-id", view_version_id);
}

yyjson_mut_val *SetCurrentViewVersionUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
