
#include "rest_catalog/objects/snapshot_references.hpp"

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

SnapshotReferences::SnapshotReferences(case_insensitive_map_t<SnapshotReference> additional_properties_p)
    : additional_properties(std::move(additional_properties_p)) {
}

SnapshotReferencesBuilder::SnapshotReferencesBuilder() {
}

SnapshotReferencesBuilder &
SnapshotReferencesBuilder::SetAdditionalProperties(case_insensitive_map_t<SnapshotReference> value) {
	additional_properties_.emplace(std::move(value));
	return *this;
}

SnapshotReferences SnapshotReferencesBuilder::Build() {
	auto result = SnapshotReferences(additional_properties_.has_value() ? std::move(*additional_properties_)
	                                                                    : case_insensitive_map_t<SnapshotReference>());
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SnapshotReferencesBuilder::TryBuild(optional<SnapshotReferences> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotReferences SnapshotReferences::FromJSON(yyjson_val *obj) {
	SnapshotReferencesBuilder builder;
	case_insensitive_map_t<SnapshotReference> additional_properties;
	size_t idx, max;
	yyjson_val *key, *val;
	yyjson_obj_foreach(obj, idx, max, key, val) {
		auto key_str = yyjson_get_str(key);
		auto tmp = SnapshotReference::FromJSON(val);
		additional_properties.emplace(key_str, std::move(tmp));
	}
	builder.SetAdditionalProperties(std::move(additional_properties));
	return builder.Build();
}

string SnapshotReferences::TryFromJSON(yyjson_val *obj, optional<SnapshotReferences> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SnapshotReferences SnapshotReferences::Copy() const {
	SnapshotReferencesBuilder builder;
	case_insensitive_map_t<SnapshotReference> additional_properties_tmp;
	for (auto &entry : additional_properties) {
		additional_properties_tmp.emplace(entry.first, entry.second.Copy());
	}
	builder.SetAdditionalProperties(std::move(additional_properties_tmp));
	return builder.Build();
}

string SnapshotReferences::Validate() const {
	string error;
	for (const auto &entry : additional_properties) {
		error = entry.second.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void SnapshotReferences::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize additional properties
	for (const auto &it : additional_properties) {
		auto &key = it.first;
		auto &value = it.second;
		yyjson_mut_val *value_obj = value.ToJSON(doc);
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_val(doc, obj, key_ptr, value_obj);
	}
}

yyjson_mut_val *SnapshotReferences::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
