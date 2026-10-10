#include "function/ducklake/catalog_conversion.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/common/file_system.hpp"
#include "duckdb/common/sql_identifier.hpp"
#include "duckdb/main/attached_database.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/transaction/transaction_context.hpp"
#include "core/metadata/puffin/iceberg_puffin_metadata.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/common/vector/vector_writer.hpp"
#include "duckdb/planner/binder.hpp"
#include "core/deletes/iceberg_deletion_vector.hpp"
#include "duckdb/common/multi_file/multi_file_reader.hpp"

namespace duckdb {
namespace iceberg {
namespace conversion {

string IdentifierSQL(const string &identifier) {
	return SQLQuotedIdentifier::ToString(identifier);
}

string StringSQL(const string &value) {
	return SQLString::ToString(value);
}

string QualifiedSQL(const vector<string> &components) {
	vector<string> quoted;
	for (auto &component : components) {
		quoted.push_back(IdentifierSQL(component));
	}
	return StringUtil::Join(quoted, ".");
}

string ValueSQL(const Value &value) {
	return value.IsNull() ? "NULL" : value.ToSQLString();
}

unique_ptr<ConversionQueryResult> Query(Connection &connection, const string &sql) {
	auto result = connection.Query(sql);
	if (result->HasError()) {
		result->ThrowError("Catalog conversion: ");
	}
	auto materialized = make_uniq<ConversionQueryResult>();
	materialized->names = result->GetNames();
	materialized->types = result->GetTypes();
	while (auto chunk = result->Fetch()) {
		for (idx_t i = 0; i < chunk->size(); i++) {
			vector<Value> row;
			for (idx_t j = 0; j < chunk->ColumnCount(); j++) {
				row.push_back(chunk->GetValue(j, i));
			}
			materialized->rows.push_back(std::move(row));
		}
	}
	return materialized;
}

void VerifyAutocommit(ClientContext &context) {
	if (!context.transaction.IsAutoCommit()) {
		throw TransactionException("Catalog conversion requires autocommit; finish the current transaction first");
	}
}

static vector<QualifiedName> ParseSelection(const Value &value, const string &option, bool qualified) {
	if (value.IsNull() || value.type().id() != LogicalTypeId::LIST) {
		throw InvalidInputException("'%s' must be a non-NULL list of table names", option);
	}
	vector<QualifiedName> result;
	for (auto &child : ListValue::GetChildren(value)) {
		if (child.IsNull()) {
			throw InvalidInputException("'%s' cannot contain NULL table names", option);
		}
		auto name = QualifiedName::Parse(child.GetValue<string>());
		if (name.Name().empty() || (qualified && name.Path().size() < 2)) {
			throw InvalidInputException("'%s' requires schema-qualified table names", option);
		}
		result.push_back(std::move(name));
	}
	return result;
}

unique_ptr<FunctionData> BindConversion(ClientContext &context, TableFunctionBindInput &input,
                                        vector<LogicalType> &types, vector<Identifier> &names) {
	auto result = make_uniq<ConversionOptions>();
	for (auto &value : input.inputs) {
		if (value.IsNull() || value.GetValue<string>().empty()) {
			throw InvalidInputException("Catalog conversion requires non-NULL attached catalog names");
		}
	}
	result->source = input.inputs[0].GetValue<string>();
	result->destination = input.inputs[1].GetValue<string>();
	if (result->source == result->destination) {
		throw InvalidInputException("Source and destination catalogs must be different");
	}
	for (auto &option : input.named_parameters) {
		auto name = StringUtil::Lower(option.first.GetIdentifierName());
		if (name == "tables") {
			result->tables = ParseSelection(option.second, name, true);
		} else if (name == "skip_tables") {
			result->skip_tables = ParseSelection(option.second, name, false);
		} else {
			if (option.second.IsNull() || option.second.GetValue<string>().empty()) {
				throw InvalidInputException("'%s' must be a nonempty metadata identifier", name);
			}
			if (name == "metadata_catalog") {
				result->metadata_catalog = option.second.GetValue<string>();
			} else if (name == "metadata_schema") {
				result->metadata_schema = option.second.GetValue<string>();
			}
		}
	}
	types.push_back(LogicalType::BIGINT);
	names.emplace_back("count");
	if (input.binder) {
		auto &destination = Catalog::GetCatalog(context, Identifier(result->destination));
		DatabaseModificationType modification;
		modification |= DatabaseModificationType::CREATE_CATALOG_ENTRY;
		input.binder->GetStatementProperties().RegisterDBModify(destination, context, modification);
		input.binder->GetStatementProperties().result_eagerness = ResultEagerness::FORCED;
	}
	return std::move(result);
}

void AddConversionOptions(TableFunction &function) {
	function.GetSignature().WithTypedKwargs("options", [&](TypedKwargs &options) {
		options.Add("tables", LogicalType::LIST(LogicalType::VARCHAR));
		options.Add("skip_tables", LogicalType::LIST(LogicalType::VARCHAR));
		options.Add("metadata_catalog", LogicalType::VARCHAR);
		options.Add("metadata_schema", LogicalType::VARCHAR);
	});
	function.verify_serialization = false;
}

static bool Matches(const QualifiedName &selection, const vector<string> &schema, const string &table) {
	auto &path = selection.Path();
	if (path.back() != Identifier(table)) {
		return false;
	}
	if (path.size() == 1) {
		return true;
	}
	if (path.size() == 2 && schema.size() > 1 && path[0] == Identifier(StringUtil::Join(schema, "."))) {
		return true;
	}
	if (path.size() != schema.size() + 1) {
		return false;
	}
	for (idx_t i = 0; i < schema.size(); i++) {
		if (path[i] != Identifier(schema[i])) {
			return false;
		}
	}
	return true;
}

bool Selected(const ConversionOptions &options, const vector<string> &schema, const string &table) {
	if (options.tables) {
		bool found = false;
		for (auto &name : *options.tables) {
			found |= Matches(name, schema, table);
		}
		if (!found) {
			return false;
		}
	}
	for (auto &name : options.skip_tables) {
		if (Matches(name, schema, table)) {
			return false;
		}
	}
	return true;
}

void VerifySelection(const ConversionOptions &options, const vector<unique_ptr<ConversionTable>> &tables) {
	if (!options.tables) {
		return;
	}
	for (auto &selection : *options.tables) {
		vector<string> schema;
		for (idx_t i = 0; i + 1 < selection.Path().size(); i++) {
			schema.push_back(selection.Path()[i].GetIdentifierName());
		}
		if (!Selected(options, schema, selection.Name().GetIdentifierName())) {
			continue;
		}
		bool found = false;
		for (auto &table : tables) {
			found |= Matches(selection, table->namespace_items, table->name);
		}
		if (!found) {
			throw InvalidInputException("Selected source table %s does not exist", selection.ToString());
		}
	}
}

static void VerifyMetadataLayout(Connection &connection, const DuckLakeMetadata &metadata) {
	map<string, string> layouts {
	    {"ducklake_metadata", "key,value,scope,scope_id"},
	    {"ducklake_snapshot", "snapshot_id,snapshot_time,schema_version,next_catalog_id,next_file_id"},
	    {"ducklake_snapshot_changes", "snapshot_id,changes_made,author,commit_message,commit_extra_info"},
	    {"ducklake_schema", "schema_id,schema_uuid,begin_snapshot,end_snapshot,schema_name,path,path_is_relative"},
	    {"ducklake_table",
	     "table_id,table_uuid,begin_snapshot,end_snapshot,schema_id,table_name,path,path_is_relative"},
	    {"ducklake_column",
	     "column_id,begin_snapshot,end_snapshot,table_id,column_order,column_name,column_type,initial_default,default_"
	     "value,nulls_allowed,parent_column,default_value_type,default_value_dialect"},
	    {"ducklake_data_file",
	     "data_file_id,table_id,begin_snapshot,end_snapshot,file_order,path,path_is_relative,file_format,record_count,"
	     "file_size_bytes,footer_size,row_id_start,partition_id,encryption_key,mapping_id,partial_max"},
	    {"ducklake_delete_file", "delete_file_id,table_id,begin_snapshot,end_snapshot,data_file_id,path,path_is_"
	                             "relative,format,delete_count,file_size_bytes,footer_size,encryption_key,partial_max"},
	    {"ducklake_partition_info", "partition_id,table_id,begin_snapshot,end_snapshot"},
	    {"ducklake_partition_column", "partition_id,table_id,partition_key_index,column_id,transform"},
	    {"ducklake_file_partition_value", "data_file_id,table_id,partition_key_index,partition_value"},
	    {"ducklake_table_stats", "table_id,record_count,next_row_id,file_size_bytes"},
	    {"ducklake_schema_versions", "begin_snapshot,schema_version,table_id"}};
	if (metadata.v11) {
		layouts["ducklake_schema"] += ",parent_schema_id";
		layouts["ducklake_data_file"] += ",row_group_count";
		layouts["ducklake_delete_file"] += ",row_group_count";
	}
	unordered_set<string> strings {"key",
	                               "value",
	                               "scope",
	                               "changes_made",
	                               "author",
	                               "commit_message",
	                               "commit_extra_info",
	                               "schema_name",
	                               "table_name",
	                               "column_name",
	                               "column_type",
	                               "initial_default",
	                               "default_value",
	                               "default_value_type",
	                               "default_value_dialect",
	                               "path",
	                               "file_format",
	                               "format",
	                               "encryption_key",
	                               "transform",
	                               "partition_value"};
	for (auto &layout : layouts) {
		auto actual = Query(connection, "SELECT * FROM " + metadata.prefix + layout.first + " LIMIT 0");
		auto columns = StringUtil::Split(layout.second, ',');
		unordered_set<string> expected(columns.begin(), columns.end());
		if (actual->names.size() != expected.size()) {
			throw InvalidInputException("Unsupported DuckLake metadata layout for %s; expected the pinned %s schema",
			                            layout.first, metadata.v11 ? "1.1-dev1" : "1.0");
		}
		for (idx_t i = 0; i < actual->names.size(); i++) {
			auto name = actual->names[i].GetIdentifierName();
			auto type = LogicalTypeId::BIGINT;
			if (strings.count(name)) {
				type = LogicalTypeId::VARCHAR;
			} else if (name == "path_is_relative" || name == "nulls_allowed") {
				type = LogicalTypeId::BOOLEAN;
			} else if (StringUtil::EndsWith(name, "_uuid")) {
				type = LogicalTypeId::UUID;
			} else if (name == "snapshot_time") {
				type = LogicalTypeId::TIMESTAMP_TZ;
			}
			if (!expected.count(name) || actual->types[i].id() != type) {
				throw InvalidInputException("Unsupported DuckLake metadata column %s.%s", layout.first, name);
			}
		}
	}
}

DuckLakeMetadata ResolveDuckLake(ClientContext &context, Connection &connection, const ConversionOptions &options,
                                 const string &catalog_name, bool destination) {
	auto &catalog = Catalog::GetCatalog(context, Identifier(catalog_name));
	if (catalog.GetCatalogType() != "ducklake") {
		throw InvalidInputException("Expected an attached DuckLake catalog: %s", catalog_name);
	}
	DuckLakeMetadata result;
	auto settings =
	    Query(connection, "SELECT catalog_type, data_path FROM ducklake_settings(" + StringSQL(catalog_name) + ")");
	if (settings->RowCount() != 1 || settings->GetValue(0, 0).ToString() != "duckdb") {
		throw InvalidInputException("Catalog conversion currently supports only DuckDB-backed DuckLake metadata");
	}
	result.data_path = settings->GetValue(1, 0).ToString();
	result.catalog = "__ducklake_metadata_" + catalog.GetName().GetIdentifierName();
	auto &attach_options = catalog.GetAttached().GetAttachOptions();
	auto catalog_option = attach_options.find("metadata_catalog");
	if (catalog_option != attach_options.end()) {
		result.catalog = catalog_option->second.GetValue<string>();
	}
	if (!options.metadata_catalog.empty()) {
		if (Identifier(options.metadata_catalog) != Identifier(result.catalog)) {
			throw InvalidInputException("metadata_catalog must match the DuckLake attachment; specify custom metadata "
			                            "names explicitly in ATTACH");
		}
	}
	auto &metadata_catalog = Catalog::GetCatalog(context, Identifier(result.catalog));
	if (metadata_catalog.GetCatalogType() != "duckdb") {
		throw InvalidInputException("DuckLake metadata catalog must be a DuckDB database");
	}
	result.catalog = metadata_catalog.GetName().GetIdentifierName();
	result.schema = "main";
	auto schema_option = attach_options.find("metadata_schema");
	if (schema_option != attach_options.end()) {
		result.schema = schema_option->second.GetValue<string>();
	}
	if (!options.metadata_schema.empty()) {
		if (Identifier(options.metadata_schema) != Identifier(result.schema)) {
			throw InvalidInputException(
			    "metadata_schema must match the DuckLake attachment; specify it explicitly in ATTACH");
		}
	}
	result.schema = metadata_catalog.GetSchema(context, Identifier(result.schema)).name.GetIdentifierName();
	result.prefix = QualifiedSQL({result.catalog, result.schema}) + ".";
	auto version = Query(connection, "SELECT value FROM " + result.prefix +
	                                     "ducklake_metadata WHERE key = 'version' AND scope IS NULL");
	if (version->RowCount() != 1) {
		throw InvalidInputException("DuckLake metadata must contain exactly one global version entry");
	}
	auto value = version->GetValue(0, 0).ToString();
	if (value != "1.0" && value != "1.1-dev1") {
		throw InvalidInputException("Catalog conversion supports DuckLake metadata 1.0 and 1.1-dev1; found %s", value);
	}
	result.v11 = value == "1.1-dev1";
	VerifyMetadataLayout(connection, result);
	auto current = Query(connection, "SELECT max(snapshot_id) FROM " + result.prefix + "ducklake_snapshot");
	auto visible =
	    Query(connection, "SELECT id::BIGINT FROM ducklake_current_snapshot(" + StringSQL(catalog_name) + ")");
	if (current->GetValue(0, 0) != visible->GetValue(0, 0)) {
		throw InvalidInputException("DuckLake metadata does not match the attached catalog's current snapshot");
	}
	if (destination) {
		if (catalog.GetAttached().IsReadOnly()) {
			throw InvalidInputException("Destination DuckLake catalog is read-only");
		}
		if (current->GetValue(0, 0).GetValue<int64_t>() != 0) {
			throw InvalidInputException("iceberg_to_ducklake requires an empty DuckLake catalog");
		}
		auto objects = Query(connection, "SELECT count(*) FROM " + result.prefix + "ducklake_table");
		if (objects->GetValue(0, 0).GetValue<int64_t>() != 0) {
			throw InvalidInputException("iceberg_to_ducklake requires an empty DuckLake catalog");
		}
	}
	return result;
}

string ResolvePath(ClientContext &context, const string &parent, const string &path, bool relative) {
	auto &fs = FileSystem::GetFileSystem(context);
	auto result = relative ? fs.JoinPath(parent, path) : path;
	if (!fs.IsPathAbsolute(result) && result.find("://") == string::npos) {
		result = fs.JoinPath(FileSystem::GetWorkingDirectory(), result);
	}
	return result;
}

string ConversionFile::Identity() const {
	return entry.data_file.file_path + "\n" +
	       (entry.data_file.content_offset ? to_string(*entry.data_file.content_offset) : string());
}

string ConversionTable::QualifiedName() const {
	auto parts = namespace_items;
	parts.push_back(name);
	return QualifiedSQL(parts);
}

string DuckLakeType(const LogicalType &type) {
	switch (type.id()) {
	case LogicalTypeId::BOOLEAN:
		return "boolean";
	case LogicalTypeId::INTEGER:
		return "int32";
	case LogicalTypeId::BIGINT:
		return "int64";
	case LogicalTypeId::FLOAT:
		return "float32";
	case LogicalTypeId::DOUBLE:
		return "float64";
	case LogicalTypeId::DECIMAL:
		return StringUtil::Format("decimal(%d,%d)", DecimalType::GetWidth(type), DecimalType::GetScale(type));
	case LogicalTypeId::DATE:
		return "date";
	case LogicalTypeId::TIME:
		return "time";
	case LogicalTypeId::TIMESTAMP:
		return "timestamp";
	case LogicalTypeId::TIMESTAMP_TZ:
		return "timestamptz";
	case LogicalTypeId::TIMESTAMP_NS:
		return "timestamp_ns";
	case LogicalTypeId::TIMESTAMP_TZ_NS:
		return "timestamptz_ns";
	case LogicalTypeId::VARCHAR:
		if (type.HasAlias() || !StringType::GetCollation(type).empty()) {
			break;
		}
		return "varchar";
	case LogicalTypeId::BLOB:
		return "blob";
	case LogicalTypeId::UUID:
		return "uuid";
	case LogicalTypeId::STRUCT:
		return "struct";
	case LogicalTypeId::LIST:
		return "list";
	case LogicalTypeId::MAP:
		return "map";
	case LogicalTypeId::VARIANT:
		return "variant";
	default:
		break;
	}
	throw InvalidInputException("Type %s has no supported lossless Iceberg/DuckLake mapping", type.ToString());
}

LogicalType IcebergType(const string &type) {
	static const pair<const char *, LogicalType> types[] = {{"boolean", LogicalType::BOOLEAN},
	                                                        {"int32", LogicalType::INTEGER},
	                                                        {"int64", LogicalType::BIGINT},
	                                                        {"float32", LogicalType::FLOAT},
	                                                        {"float64", LogicalType::DOUBLE},
	                                                        {"date", LogicalType::DATE},
	                                                        {"time", LogicalType::TIME},
	                                                        {"timestamp", LogicalType::TIMESTAMP},
	                                                        {"timestamp_us", LogicalType::TIMESTAMP},
	                                                        {"timestamptz", LogicalType::TIMESTAMP_TZ},
	                                                        {"timestamp_ns", LogicalType::TIMESTAMP_NS},
	                                                        {"timestamptz_ns", LogicalType::TIMESTAMP_TZ_NS},
	                                                        {"varchar", LogicalType::VARCHAR},
	                                                        {"blob", LogicalType::BLOB},
	                                                        {"uuid", LogicalType::UUID},
	                                                        {"variant", LogicalType::VARIANT()}};
	for (auto &entry : types) {
		if (type == entry.first) {
			return entry.second;
		}
	}
	if (StringUtil::StartsWith(type, "decimal(") && StringUtil::EndsWith(type, ")")) {
		auto parts = StringUtil::Split(type.substr(8, type.size() - 9), ',');
		if (parts.size() != 2) {
			throw InvalidInputException("Invalid DuckLake decimal type '%s'", type);
		}
		auto width = Value(parts[0]).GetValue<uint8_t>();
		auto scale = Value(parts[1]).GetValue<uint8_t>();
		if (!width || width > 38 || scale > width) {
			throw InvalidInputException("Invalid DuckLake decimal type '%s'", type);
		}
		return LogicalType::DECIMAL(width, scale);
	}
	throw InvalidInputException("DuckLake type '%s' has no supported lossless Iceberg mapping", type);
}

static void VerifyColumn(const IcebergColumnDefinition &column, unordered_set<int32_t> &ids) {
	DuckLakeType(column.type);
	if (column.id <= 0 || column.id >= 2147483448 || !ids.insert(column.id).second) {
		throw InvalidInputException("Column %s has an invalid or reserved field ID %d", column.name, column.id);
	}
	unordered_set<string> names;
	for (auto &child : column.GetChildren()) {
		if (!names.insert(StringUtil::Lower(child->name)).second) {
			throw InvalidInputException("Case-colliding nested column names cannot be converted: %s", child->name);
		}
		VerifyColumn(*child, ids);
	}
}

void VerifySchema(const IcebergTableSchema &schema, bool to_ducklake) {
	if (schema.columns.empty()) {
		throw InvalidInputException("Zero-column tables have no supported Iceberg/DuckLake mapping");
	}
	if (!schema.identifier_field_ids.empty() && to_ducklake) {
		throw InvalidInputException("Iceberg identifier fields cannot be represented in DuckLake");
	}
	unordered_set<int32_t> ids;
	unordered_set<string> names;
	for (auto &column : schema.columns) {
		if (!names.insert(StringUtil::Lower(column->name)).second) {
			throw InvalidInputException("Case-colliding column names cannot be converted: %s", column->name);
		}
		VerifyColumn(*column, ids);
	}
}

static void VerifyEvolution(const ConversionTable &table) {
	map<int32_t, pair<LogicalType, int32_t>> columns;
	auto visit = [&](auto &&self, const IcebergColumnDefinition &column, int32_t parent) -> void {
		auto existing = columns.find(column.id);
		if (existing != columns.end()) {
			auto &old = existing->second.first;
			auto &current = column.type;
			bool compatible = old.id() == current.id();
			if (old.id() == LogicalTypeId::DECIMAL && current.id() == LogicalTypeId::DECIMAL) {
				compatible = DecimalType::GetScale(old) == DecimalType::GetScale(current) &&
				             DecimalType::GetWidth(old) <= DecimalType::GetWidth(current);
			}
			compatible |= (old.id() == LogicalTypeId::INTEGER && current.id() == LogicalTypeId::BIGINT) ||
			              (old.id() == LogicalTypeId::FLOAT && current.id() == LogicalTypeId::DOUBLE);
			if (!compatible || existing->second.second != parent) {
				throw InvalidInputException(
				    "%s: column %s (field ID %d) has incompatible schema evolution from %s to %s",
				    table.QualifiedName(), column.name, column.id, old.ToString(), current.ToString());
			}
		}
		columns[column.id] = {column.type, parent};
		for (auto &child : column.GetChildren()) {
			self(self, *child, column.id);
		}
	};
	for (auto &state : table.states) {
		for (auto &column : table.metadata.GetSchemaFromId(state.schema_id).columns) {
			visit(visit, *column, 0);
		}
	}
}

string DuckLakeTransform(const IcebergTransform &transform) {
	switch (transform.Type()) {
	case IcebergTransformType::IDENTITY:
		return transform.RawType();
	case IcebergTransformType::BUCKET:
		return StringUtil::Format("bucket(%llu)", transform.GetBucketModulo());
	case IcebergTransformType::YEAR:
	case IcebergTransformType::MONTH:
	case IcebergTransformType::DAY:
	case IcebergTransformType::HOUR:
		return "epoch_" + transform.RawType();
	default:
		throw InvalidInputException("Iceberg partition transform '%s' cannot be represented in DuckLake",
		                            transform.RawType());
	}
}

IcebergTransform IcebergPartitionTransform(const string &transform) {
	auto name = transform;
	if (StringUtil::StartsWith(name, "bucket(") && StringUtil::EndsWith(name, ")")) {
		name = "bucket[" + name.substr(7, name.size() - 8) + "]";
	}
	if (StringUtil::StartsWith(name, "epoch_")) {
		name = name.substr(6);
	} else if (name == "month" || name == "day" || name == "hour") {
		throw InvalidInputException("DuckLake calendar-part transform '%s' is not an Iceberg epoch transform", name);
	}
	auto result = IcebergTransform(name);
	DuckLakeTransform(result);
	return result;
}

string ReferencedDataFile(const ConversionFile &file) {
	auto &data = file.entry.data_file;
	if (data.referenced_data_file) {
		return *data.referenced_data_file;
	}
	auto lower = data.lower_bounds.find(2147483546);
	auto upper = data.upper_bounds.find(2147483546);
	if (lower == data.lower_bounds.end() || upper == data.upper_bounds.end() || lower->second.IsNull() ||
	    lower->second != upper->second) {
		throw InvalidInputException("Position delete file '%s' must provably reference exactly one data file",
		                            data.file_path);
	}
	return lower->second.GetValue<string>();
}

static void VerifyParquetFields(Connection &connection, const ConversionTable &table, const IcebergDataFile &data) {
	auto schema =
	    Query(connection, "SELECT name, field_id, num_children, converted_type, logical_type FROM parquet_schema(" +
	                          StringSQL(data.file_path) + ")");
	idx_t row = 0;
	bool delete_path = false;
	bool delete_position = false;
	auto visit = [&](auto &&self, LogicalTypeId parent, bool inside_variant) -> void {
		if (row >= schema->RowCount()) {
			throw InvalidInputException("Invalid Parquet schema in %s", data.file_path);
		}
		auto index = row++;
		auto id_value = schema->GetValue(1, index);
		auto child_count = schema->GetValue(2, index);
		auto children = child_count.IsNull() ? 0 : child_count.GetValue<int64_t>();
		auto name = schema->GetValue(0, index).ToString();
		auto role = LogicalTypeId::INVALID;
		auto converted = schema->GetValue(3, index).ToString();
		if (converted == "LIST") {
			role = LogicalTypeId::LIST;
		} else if (converted == "MAP") {
			role = LogicalTypeId::MAP;
		} else if (StringUtil::Contains(StringUtil::Lower(schema->GetValue(4, index).ToString()), "variant")) {
			role = LogicalTypeId::VARIANT;
		}
		bool wrapper = children > 0 && ((parent == LogicalTypeId::LIST && name == "list") ||
		                                (parent == LogicalTypeId::MAP && name == "key_value"));
		if (index != 0 && !inside_variant && !wrapper && id_value.IsNull()) {
			throw InvalidInputException("%s: %s has columns without field IDs; name mappings require rewriting",
			                            table.QualifiedName(), data.file_path);
		}
		if (!id_value.IsNull() && !inside_variant) {
			auto id = id_value.GetValue<int64_t>();
			delete_path |= id == MultiFileReader::DELETE_FILE_PATH_FIELD_ID;
			delete_position |= id == MultiFileReader::DELETE_POS_FIELD_ID;
			if (data.content == IcebergManifestEntryContentType::DATA && (id == 2147483540 || id == 2147483539)) {
				throw InvalidInputException("%s: %s contains embedded row lineage/sequence columns that cannot "
				                            "be remapped without rewriting",
				                            table.QualifiedName(), data.file_path);
			}
			auto column = table.metadata.FindColumnByFieldId(NumericCast<int32_t>(id));
			if (column) {
				role = column->type.id();
			}
		}
		for (int64_t i = 0; i < children; i++) {
			self(self, role, inside_variant || role == LogicalTypeId::VARIANT);
		}
	};
	visit(visit, LogicalTypeId::INVALID, false);
	if (row != schema->RowCount()) {
		throw InvalidInputException("Invalid Parquet schema in %s", data.file_path);
	}
	if (data.content == IcebergManifestEntryContentType::POSITION_DELETES && (!delete_path || !delete_position)) {
		throw InvalidInputException(
		    "%s: position delete file %s has incompatible Parquet field IDs; Iceberg requires "
		    "file_path=2147483546 and pos=2147483545. Rewrite the deletes with an Iceberg-compatible writer.",
		    table.QualifiedName(), data.file_path);
	}
}

void VerifyFiles(ClientContext &context, Connection &connection, const ConversionTable &table) {
	unordered_set<string> checked;
	auto &fs = FileSystem::GetFileSystem(context);
	for (auto &state : table.states) {
		unordered_set<string> data_paths;
		for (auto &file : state.files) {
			if (file.entry.data_file.content == IcebergManifestEntryContentType::DATA) {
				if (!data_paths.insert(file.entry.data_file.file_path).second) {
					throw InvalidInputException("%s, snapshot %lld: multiple active references to data file %s",
					                            table.QualifiedName(), state.source_id, file.entry.data_file.file_path);
				}
			}
		}
		unordered_set<string> deleted_paths;
		for (auto &file : state.files) {
			auto &data = file.entry.data_file;
			if (data.content == IcebergManifestEntryContentType::EQUALITY_DELETES) {
				throw InvalidInputException("%s, snapshot %lld: equality deletes require rewriting files",
				                            table.QualifiedName(), state.source_id);
			}
			if (data.content != IcebergManifestEntryContentType::DATA) {
				auto path = ReferencedDataFile(file);
				if (!data_paths.count(path)) {
					throw InvalidInputException("%s, snapshot %lld: delete file %s references an inactive data file",
					                            table.QualifiedName(), state.source_id, data.file_path);
				}
				if (!deleted_paths.insert(path).second) {
					throw InvalidInputException("%s, snapshot %lld: multiple active delete files reference %s",
					                            table.QualifiedName(), state.source_id, path);
				}
			}
			if (!checked.insert(file.Identity()).second) {
				continue;
			}
			if (data.record_count < 0 || data.file_size_in_bytes < 0) {
				throw InvalidInputException("%s: invalid counts in %s", table.QualifiedName(), data.file_path);
			}
			auto handle = fs.OpenFile(data.file_path, FileFlags::FILE_FLAGS_READ);
			if (handle->GetFileSize() != data.file_size_in_bytes) {
				throw InvalidInputException("%s: file size changed for %s", table.QualifiedName(), data.file_path);
			}
			if (StringUtil::CIEquals(data.file_format, "puffin")) {
				if (!data.content_offset || !data.content_size_in_bytes || *data.content_offset < 0 ||
				    *data.content_size_in_bytes <= 0 ||
				    *data.content_offset > data.file_size_in_bytes - *data.content_size_in_bytes) {
					throw InvalidInputException("%s: invalid deletion-vector range in %s", table.QualifiedName(),
					                            data.file_path);
				}
				vector<data_t> bytes(NumericCast<idx_t>(*data.content_size_in_bytes));
				handle->Read(bytes.data(), bytes.size(), *data.content_offset);
				auto bitmap = IcebergDeletionVectorData::FromBlob({data.file_path, data.content_offset}, bytes.data(),
				                                                  bytes.size());
				uint64_t count = 0;
				for (auto &part : bitmap->bitmaps) {
					count += part.second.cardinality();
				}
				if (count != NumericCast<uint64_t>(data.record_count)) {
					throw InvalidInputException("%s: deletion-vector cardinality mismatch in %s", table.QualifiedName(),
					                            data.file_path);
				}
				continue;
			}
			if (!StringUtil::CIEquals(data.file_format, "parquet")) {
				throw InvalidInputException("%s: file %s uses unsupported format %s", table.QualifiedName(),
				                            data.file_path, data.file_format);
			}
			VerifyParquetFields(connection, table, data);
			auto counts =
			    Query(connection, "SELECT num_rows FROM parquet_file_metadata(" + StringSQL(data.file_path) + ")");
			if (counts->RowCount() != 1 || counts->GetValue(0, 0).GetValue<int64_t>() != data.record_count) {
				throw InvalidInputException("%s: Parquet row count mismatch in %s", table.QualifiedName(),
				                            data.file_path);
			}
			if (data.content == IcebergManifestEntryContentType::POSITION_DELETES) {
				auto reference = ReferencedDataFile(file);
				auto invalid = Query(connection, "SELECT count(*) FROM read_parquet(" + StringSQL(data.file_path) +
				                                     ") WHERE file_path IS DISTINCT FROM " + StringSQL(reference) +
				                                     " OR pos IS NULL OR pos < 0");
				if (invalid->GetValue(0, 0).GetValue<int64_t>() != 0) {
					throw InvalidInputException("%s: position delete file %s does not contain the absolute referenced "
					                            "data path. Create DuckLake files with an absolute DATA_PATH; existing "
					                            "relative-path deletes require rewriting.",
					                            table.QualifiedName(), data.file_path);
				}
			}
		}
	}
}

struct ConversionExecutionState : public GlobalTableFunctionState {
	bool finished = false;
};

static unique_ptr<GlobalTableFunctionState> InitConversion(ClientContext &, TableFunctionInitInput &) {
	return make_uniq<ConversionExecutionState>();
}

static void ExecuteConversion(ClientContext &context, TableFunctionInput &input, DataChunk &output, bool to_ducklake) {
	auto &state = input.global_state->Cast<ConversionExecutionState>();
	if (state.finished) {
		return;
	}
	VerifyAutocommit(context);
	auto &options = input.bind_data->Cast<ConversionOptions>();
	Connection connection(DatabaseInstance::GetDatabase(context));
	connection.BeginTransaction();
	try {
		auto &conversion_context = *connection.context;
		auto metadata = ResolveDuckLake(conversion_context, connection, options,
		                                to_ducklake ? options.destination : options.source, to_ducklake);
		auto plan = to_ducklake ? ReadIceberg(conversion_context, connection, options)
		                        : ReadDuckLake(conversion_context, connection, options, metadata);
		auto &tables = plan.tables;
		VerifySelection(options, tables);
		unordered_set<string> table_names;
		map<string, string> namespace_names;
		auto verify_namespace = [&](const vector<string> &parts) {
			vector<string> prefix;
			for (auto &part : parts) {
				prefix.push_back(part);
				auto name = QualifiedSQL(prefix);
				auto key = to_ducklake ? StringUtil::Lower(name) : name;
				auto inserted = namespace_names.emplace(key, name);
				if (!inserted.second && inserted.first->second != name) {
					throw InvalidInputException("Case-colliding namespaces cannot be converted to DuckLake: %s", name);
				}
			}
		};
		for (auto &parts : plan.namespaces) {
			verify_namespace(parts);
		}
		for (auto &table : tables) {
			verify_namespace(table->namespace_items);
			auto name = table->QualifiedName();
			if (!table_names.insert(to_ducklake ? StringUtil::Lower(name) : name).second) {
				throw InvalidInputException("Duplicate or case-colliding destination table: %s", name);
			}
		}
		if (to_ducklake && !metadata.v11) {
			for (auto &parts : plan.namespaces) {
				if (parts.size() > 1) {
					throw InvalidInputException("Nested namespaces require DuckLake metadata 1.1-dev1");
				}
			}
			for (auto &table : tables) {
				if (table->namespace_items.size() > 1) {
					throw InvalidInputException("Nested namespaces require DuckLake metadata 1.1-dev1");
				}
			}
		}
		std::sort(tables.begin(), tables.end(),
		          [](const unique_ptr<ConversionTable> &a, const unique_ptr<ConversionTable> &b) {
			          return a->QualifiedName() < b->QualifiedName();
		          });
		for (auto &table : tables) {
			VerifyEvolution(*table);
			VerifyFiles(conversion_context, connection, *table);
		}
		if (to_ducklake) {
			WriteDuckLake(connection, metadata, plan);
		} else {
			WriteIceberg(conversion_context, connection, options, plan);
		}
		connection.Commit();
		output.SetChildCardinality(1);
		auto writer = FlatVector::Writer<int64_t>(output.data[0], 1);
		writer.WriteValue(NumericCast<int64_t>(tables.size()));
		state.finished = true;
	} catch (...) {
		connection.Query("ROLLBACK");
		throw;
	}
}

static void ToDuckLake(ClientContext &context, TableFunctionInput &input, DataChunk &output) {
	ExecuteConversion(context, input, output, true);
}

static void ToIceberg(ClientContext &context, TableFunctionInput &input, DataChunk &output) {
	ExecuteConversion(context, input, output, false);
}

TableFunctionSet ConversionFunction(bool to_ducklake) {
	TableFunctionSet result(to_ducklake ? "iceberg_to_ducklake" : "ducklake_to_iceberg");
	TableFunction function(FunctionSignature()
	                           .AddPositionalOnly("source_catalog", LogicalType::VARCHAR)
	                           .AddPositionalOnly("destination_catalog", LogicalType::VARCHAR),
	                       to_ducklake ? ToDuckLake : ToIceberg, BindConversion, InitConversion);
	AddConversionOptions(function);
	result.AddFunction(function);
	return result;
}

} // namespace conversion
} // namespace iceberg
} // namespace duckdb
