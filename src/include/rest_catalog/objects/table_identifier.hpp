
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/namespace.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TableIdentifierBuilder;

class TableIdentifier {
public:
	TableIdentifier(const TableIdentifier &) = delete;
	TableIdentifier &operator=(const TableIdentifier &) = delete;
	TableIdentifier(TableIdentifier &&) = default;
	TableIdentifier &operator=(TableIdentifier &&) = default;

private:
	friend class TableIdentifierBuilder;
	friend class GeneratedObjectAccess;
	TableIdentifier();

public:
	// Deserialization
	static TableIdentifier FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	TableIdentifier Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	Namespace _namespace;
	string name;
};

class TableIdentifierBuilder {
public:
	TableIdentifierBuilder();
	TableIdentifierBuilder &SetNamespace(Namespace value);
	TableIdentifierBuilder &SetName(string value);
	string TryBuild(TableIdentifier &result);
	TableIdentifier Build();

private:
	TableIdentifier result_;
	bool has__namespace_ = false;
	bool has_name_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
