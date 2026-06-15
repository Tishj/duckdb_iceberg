
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/base_update.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class UpgradeFormatVersionUpdateBuilder;

class UpgradeFormatVersionUpdate {
public:
	UpgradeFormatVersionUpdate(const UpgradeFormatVersionUpdate &) = delete;
	UpgradeFormatVersionUpdate &operator=(const UpgradeFormatVersionUpdate &) = delete;
	UpgradeFormatVersionUpdate(UpgradeFormatVersionUpdate &&) = default;
	UpgradeFormatVersionUpdate &operator=(UpgradeFormatVersionUpdate &&) = delete;

private:
	friend class UpgradeFormatVersionUpdateBuilder;
	UpgradeFormatVersionUpdate(BaseUpdate base_update_p, int32_t format_version_p);

public:
	// Deserialization
	static UpgradeFormatVersionUpdate FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, optional<UpgradeFormatVersionUpdate> &result);
	string Validate() const;

	// Copy
	UpgradeFormatVersionUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	BaseUpdate base_update;
	int32_t format_version;
};

class UpgradeFormatVersionUpdateBuilder {
public:
	UpgradeFormatVersionUpdateBuilder();
	UpgradeFormatVersionUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	UpgradeFormatVersionUpdateBuilder &SetFormatVersion(int32_t value);
	string TryBuild(optional<UpgradeFormatVersionUpdate> &result);
	UpgradeFormatVersionUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int32_t> format_version_;
	bool has_format_version_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
