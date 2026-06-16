
#include "rest_catalog/objects/view_history_entry.hpp"

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

ViewHistoryEntry::ViewHistoryEntry(int32_t version_id_p, int64_t timestamp_ms_p)
    : version_id(std::move(version_id_p)), timestamp_ms(std::move(timestamp_ms_p)) {
}
ViewHistoryEntry::ViewHistoryEntry(const ViewHistoryEntry &other)
    : version_id(other.version_id), timestamp_ms(other.timestamp_ms) {
}
ViewHistoryEntry::ViewHistoryEntry(ViewHistoryEntry &&other)
    : ViewHistoryEntry(static_cast<const ViewHistoryEntry &>(other)) {
}

ViewHistoryEntryBuilder::ViewHistoryEntryBuilder() {
}

ViewHistoryEntryBuilder &ViewHistoryEntryBuilder::SetVersionId(int32_t value) {
	version_id_.emplace(std::move(value));
	has_version_id_ = true;
	return *this;
}

ViewHistoryEntryBuilder &ViewHistoryEntryBuilder::SetTimestampMs(int64_t value) {
	timestamp_ms_.emplace(std::move(value));
	has_timestamp_ms_ = true;
	return *this;
}

ViewHistoryEntry ViewHistoryEntryBuilder::Build() {
	if (!has_version_id_) {
		throw InvalidInputException("ViewHistoryEntry required property 'version-id' is missing");
	}
	if (!has_timestamp_ms_) {
		throw InvalidInputException("ViewHistoryEntry required property 'timestamp-ms' is missing");
	}
	auto result = ViewHistoryEntry(std::move(*version_id_), std::move(*timestamp_ms_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewHistoryEntryBuilder::TryBuild(optional<ViewHistoryEntry> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string ViewHistoryEntry::TryFromJSON(yyjson_val *obj, ViewHistoryEntryBuilder &builder) {
	try {
		auto version_id_val = yyjson_obj_get(obj, "version-id");
		if (!version_id_val) {
			throw InvalidInputException("ViewHistoryEntry required property 'version-id' is missing");
		} else {
			int32_t version_id;
			if (yyjson_is_int(version_id_val)) {
				version_id = yyjson_get_int(version_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ViewHistoryEntry property 'version_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(version_id_val)));
			}
			builder.SetVersionId(std::move(version_id));
		}
		auto timestamp_ms_val = yyjson_obj_get(obj, "timestamp-ms");
		if (!timestamp_ms_val) {
			throw InvalidInputException("ViewHistoryEntry required property 'timestamp-ms' is missing");
		} else {
			int64_t timestamp_ms;
			if (yyjson_is_sint(timestamp_ms_val)) {
				timestamp_ms = yyjson_get_sint(timestamp_ms_val);
			} else if (yyjson_is_uint(timestamp_ms_val)) {
				timestamp_ms = yyjson_get_uint(timestamp_ms_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "ViewHistoryEntry property 'timestamp_ms' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(timestamp_ms_val)));
			}
			builder.SetTimestampMs(std::move(timestamp_ms));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewHistoryEntry ViewHistoryEntry::FromJSON(yyjson_val *obj) {
	ViewHistoryEntryBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

ViewHistoryEntry ViewHistoryEntry::Copy() const {
	return ViewHistoryEntry(*this);
}

string ViewHistoryEntry::Validate() const {
	string error;
	return "";
}

void ViewHistoryEntry::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: version-id
	yyjson_mut_obj_add_int(doc, obj, "version-id", version_id);

	// Serialize: timestamp-ms
	yyjson_mut_obj_add_sint(doc, obj, "timestamp-ms", timestamp_ms);
}

yyjson_mut_val *ViewHistoryEntry::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
