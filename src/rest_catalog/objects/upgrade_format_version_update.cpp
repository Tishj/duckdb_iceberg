
#include "rest_catalog/objects/upgrade_format_version_update.hpp"

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

UpgradeFormatVersionUpdate::UpgradeFormatVersionUpdate(BaseUpdate base_update_p, int32_t format_version_p)
    : base_update(std::move(base_update_p)), format_version(std::move(format_version_p)) {
}
UpgradeFormatVersionUpdate::UpgradeFormatVersionUpdate(const UpgradeFormatVersionUpdate &other)
    : base_update(other.base_update.Copy()), format_version(other.format_version) {
}
UpgradeFormatVersionUpdate::UpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate &&other)
    : UpgradeFormatVersionUpdate(static_cast<const UpgradeFormatVersionUpdate &>(other)) {
}

UpgradeFormatVersionUpdateBuilder::UpgradeFormatVersionUpdateBuilder() {
}

UpgradeFormatVersionUpdateBuilder &UpgradeFormatVersionUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

UpgradeFormatVersionUpdateBuilder &UpgradeFormatVersionUpdateBuilder::SetFormatVersion(int32_t value) {
	format_version_.emplace(std::move(value));
	has_format_version_ = true;
	return *this;
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdateBuilder::Build() {
	if (!has_format_version_) {
		throw InvalidInputException("UpgradeFormatVersionUpdate required property 'format-version' is missing");
	}
	auto result = UpgradeFormatVersionUpdate(std::move(*base_update_), std::move(*format_version_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string UpgradeFormatVersionUpdateBuilder::TryBuild(optional<UpgradeFormatVersionUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string UpgradeFormatVersionUpdate::TryFromJSON(yyjson_val *obj, UpgradeFormatVersionUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto format_version_val = yyjson_obj_get(obj, "format-version");
		if (!format_version_val) {
			throw InvalidInputException("UpgradeFormatVersionUpdate required property 'format-version' is missing");
		} else {
			int32_t format_version;
			if (yyjson_is_int(format_version_val)) {
				format_version = yyjson_get_int(format_version_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "UpgradeFormatVersionUpdate property 'format_version' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(format_version_val)));
			}
			builder.SetFormatVersion(std::move(format_version));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdate::FromJSON(yyjson_val *obj) {
	UpgradeFormatVersionUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

UpgradeFormatVersionUpdate UpgradeFormatVersionUpdate::Copy() const {
	return UpgradeFormatVersionUpdate(*this);
}

string UpgradeFormatVersionUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
