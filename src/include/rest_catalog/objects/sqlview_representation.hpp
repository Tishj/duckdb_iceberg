
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class SQLViewRepresentation {
public:
	SQLViewRepresentation();
	SQLViewRepresentation(const SQLViewRepresentation &) = delete;
	SQLViewRepresentation &operator=(const SQLViewRepresentation &) = delete;
	SQLViewRepresentation(SQLViewRepresentation &&) = default;
	SQLViewRepresentation &operator=(SQLViewRepresentation &&) = default;

public:
	// Deserialization
	static SQLViewRepresentation FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	SQLViewRepresentation Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string type;
	string sql;
	string dialect;
};

class SQLViewRepresentationBuilder {
public:
	SQLViewRepresentationBuilder();
	SQLViewRepresentationBuilder &SetType(string value);
	SQLViewRepresentationBuilder &SetSql(string value);
	SQLViewRepresentationBuilder &SetDialect(string value);
	string TryBuild(SQLViewRepresentation &result);
	SQLViewRepresentation Build();

private:
	SQLViewRepresentation result_;
	bool has_type_ = false;
	bool has_sql_ = false;
	bool has_dialect_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
