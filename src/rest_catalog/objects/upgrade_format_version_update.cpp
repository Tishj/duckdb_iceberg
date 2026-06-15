
#include "rest_catalog/objects/upgrade_format_version_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

UpgradeFormatVersionUpdate::UpgradeFormatVersionUpdate() : base_update(GeneratedObjectAccess::Create<BaseUpdate>()) {
}

UpgradeFormatVersionUpdateBuilder::UpgradeFormatVersionUpdateBuilder() {
}

UpgradeFormatVersionUpdateBuilder &UpgradeFormatVersionUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	result_.base_update = std::move(value);
	return *this;
}

UpgradeFormatVersionUpdateBuilder &UpgradeFormatVersionUpdateBuilder::SetFormatVersion(int32_t value) {
	result_.format_version = std::move(value);
	has_format_version_ = true;
	return *this;
}

string UpgradeFormatVersionUpdateBuilder::TryBuild(UpgradeFormatVersionUpdate &result) {
	if (!has_format_version_) {
		return "UpgradeFormatVersionUpdate required property 'format-version' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdateBuilder::Build() {
	UpgradeFormatVersionUpdate result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdate::FromJSON(yyjson_val *obj) {
	UpgradeFormatVersionUpdate res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdate::Copy() const {
	UpgradeFormatVersionUpdate res;
	res.base_update = base_update.Copy();
	res.format_version = format_version;
	return res;
}

string UpgradeFormatVersionUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

string UpgradeFormatVersionUpdate::TryFromJSON(yyjson_val *obj) {
	string error;
	error = base_update.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto format_version_val = yyjson_obj_get(obj, "format-version");
	if (!format_version_val) {
		return "UpgradeFormatVersionUpdate required property 'format-version' is missing";
	} else {
		if (yyjson_is_int(format_version_val)) {
			format_version = yyjson_get_int(format_version_val);
		} else {
			return StringUtil::Format(
			    "UpgradeFormatVersionUpdate property 'format_version' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(format_version_val));
		}
	}
	return Validate();
}

void UpgradeFormatVersionUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: format-version
	yyjson_mut_obj_add_int(doc, obj, "format-version", format_version);
}

yyjson_mut_val *UpgradeFormatVersionUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
