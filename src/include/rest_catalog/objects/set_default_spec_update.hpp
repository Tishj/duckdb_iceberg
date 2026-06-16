
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

class SetDefaultSpecUpdateBuilder;

class SetDefaultSpecUpdate {
public:
	SetDefaultSpecUpdate(const SetDefaultSpecUpdate &);
	SetDefaultSpecUpdate &operator=(const SetDefaultSpecUpdate &) = delete;
	SetDefaultSpecUpdate(SetDefaultSpecUpdate &&);
	SetDefaultSpecUpdate &operator=(SetDefaultSpecUpdate &&) = delete;

private:
	friend class SetDefaultSpecUpdateBuilder;
	SetDefaultSpecUpdate(BaseUpdate base_update_p, int32_t spec_id_p);

public:
	// Deserialization
	static SetDefaultSpecUpdate FromJSON(yyjson_val *obj);
	static optional<string> TryFromJSON(yyjson_val *obj, SetDefaultSpecUpdateBuilder &builder);
	optional<string> Validate() const;

	// Copy
	SetDefaultSpecUpdate Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const BaseUpdate base_update;
	const int32_t spec_id;
};

class SetDefaultSpecUpdateBuilder {
public:
	SetDefaultSpecUpdateBuilder();
	SetDefaultSpecUpdateBuilder &SetBaseUpdate(BaseUpdate value);
	SetDefaultSpecUpdateBuilder &SetSpecId(int32_t value);
	optional<string> TryBuild(optional<SetDefaultSpecUpdate> &result);
	SetDefaultSpecUpdate Build();

private:
	optional<BaseUpdate> base_update_;
	optional<int32_t> spec_id_;
	bool has_spec_id_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
