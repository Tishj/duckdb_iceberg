#pragma once

#include "duckdb/function/table_function.hpp"
#include "duckdb/main/connection.hpp"
#include "duckdb/parser/qualified_name.hpp"
#include "core/metadata/iceberg_table_metadata.hpp"
#include "core/metadata/manifest/iceberg_manifest_list.hpp"

namespace duckdb {
namespace iceberg {
namespace conversion {

struct ConversionOptions : public TableFunctionData {
	string source;
	string destination;
	string metadata_catalog;
	string metadata_schema;
	optional<vector<QualifiedName>> tables;
	vector<QualifiedName> skip_tables;
};

struct ConversionFile {
	IcebergManifestEntry entry;
	int32_t spec_id = 0;
	int32_t schema_id = 0;
	optional<int64_t> row_id_start;
	optional<int64_t> footer_size;

	string Identity() const;
};

struct ConversionState {
	int64_t source_id = 0;
	timestamp_t timestamp;
	int32_t schema_id = 0;
	int32_t spec_id = 0;
	bool synthetic = false;
	vector<ConversionFile> files;
	string provenance;
};

struct ConversionTable {
	vector<string> namespace_items;
	string name;
	string source_uuid;
	IcebergTableMetadata metadata {IcebergTableMetadataSchemas {}};
	vector<ConversionState> states;
	unordered_set<uint64_t> calendar_year_fields;

	string QualifiedName() const;
};

struct DuckLakeMetadata {
	string catalog;
	string schema;
	string prefix;
	string data_path;
	bool v11 = false;
};

struct ConversionPlan {
	vector<unique_ptr<ConversionTable>> tables;
	vector<vector<string>> namespaces;
};

struct ConversionQueryResult {
	vector<Identifier> names;
	vector<LogicalType> types;
	vector<vector<Value>> rows;
	idx_t RowCount() const {
		return rows.size();
	}
	Value GetValue(idx_t column, idx_t row) const {
		return rows.at(row).at(column);
	}
	const vector<Identifier> &GetNames() const {
		return names;
	}
};

unique_ptr<FunctionData> BindConversion(ClientContext &context, TableFunctionBindInput &input,
                                        vector<LogicalType> &types, vector<Identifier> &names);
void AddConversionOptions(TableFunction &function);
void VerifyAutocommit(ClientContext &context);
unique_ptr<ConversionQueryResult> Query(Connection &connection, const string &sql);
string IdentifierSQL(const string &identifier);
string StringSQL(const string &value);
string QualifiedSQL(const vector<string> &components);
string ValueSQL(const Value &value);
bool Selected(const ConversionOptions &options, const vector<string> &schema, const string &table);
void VerifySelection(const ConversionOptions &options, const vector<unique_ptr<ConversionTable>> &tables);
DuckLakeMetadata ResolveDuckLake(ClientContext &context, Connection &connection, const ConversionOptions &options,
                                 const string &catalog_name, bool destination);
string ResolvePath(ClientContext &context, const string &parent, const string &path, bool relative);
void VerifySchema(const IcebergTableSchema &schema, bool to_ducklake);
string DuckLakeType(const LogicalType &type);
LogicalType IcebergType(const string &type);
string DuckLakeTransform(const IcebergTransform &transform);
IcebergTransform IcebergPartitionTransform(const string &transform);
void VerifyFiles(ClientContext &context, Connection &connection, const ConversionTable &table);
string ReferencedDataFile(const ConversionFile &file);
ConversionPlan ReadIceberg(ClientContext &context, Connection &connection, const ConversionOptions &options);
ConversionPlan ReadDuckLake(ClientContext &context, Connection &connection, const ConversionOptions &options,
                            const DuckLakeMetadata &metadata);
void WriteDuckLake(Connection &connection, const DuckLakeMetadata &metadata, const ConversionPlan &plan);
void WriteIceberg(ClientContext &context, Connection &connection, const ConversionOptions &options,
                  const ConversionPlan &plan);
TableFunctionSet ConversionFunction(bool to_ducklake);

} // namespace conversion
} // namespace iceberg
} // namespace duckdb
