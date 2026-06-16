
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/sqlview_representation.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class ViewRepresentationBuilder;

class ViewRepresentation {
public:
	ViewRepresentation(const ViewRepresentation &);
	ViewRepresentation &operator=(const ViewRepresentation &) = delete;
	ViewRepresentation(ViewRepresentation &&);
	ViewRepresentation &operator=(ViewRepresentation &&) = delete;

private:
	friend class ViewRepresentationBuilder;
	ViewRepresentation(optional<SQLViewRepresentation> sqlview_representation_p);

public:
	// Deserialization
	static ViewRepresentation FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, ViewRepresentationBuilder &builder);
	string Validate() const;

	// Copy
	ViewRepresentation Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const optional<SQLViewRepresentation> sqlview_representation;
};

class ViewRepresentationBuilder {
public:
	ViewRepresentationBuilder();
	ViewRepresentationBuilder &SetSqlviewRepresentation(SQLViewRepresentation value);
	string TryBuild(optional<ViewRepresentation> &result);
	ViewRepresentation Build();

private:
	optional<SQLViewRepresentation> sqlview_representation_;
};

} // namespace rest_api_objects
} // namespace duckdb
