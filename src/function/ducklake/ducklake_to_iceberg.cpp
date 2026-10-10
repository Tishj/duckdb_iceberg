#include "function/ducklake/catalog_conversion.hpp"
#include "function/iceberg_functions.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/iceberg_schema_set.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/api/iceberg_create_table_request.hpp"
#include "common/iceberg_utils.hpp"
#include "core/metadata/puffin/iceberg_puffin_metadata.hpp"
#include "duckdb/catalog/catalog_entry/copy_function_catalog_entry.hpp"
#include "duckdb/common/json_document.hpp"
#include "duckdb/common/types/uuid.hpp"
#include "duckdb/main/database.hpp"
#include "duckdb/main/table_description.hpp"
#include "duckdb/parser/parser.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"

namespace duckdb {
namespace iceberg {
namespace conversion {

class MetadataRows {
public:
	MetadataRows(Connection &connection, const string &sql) : result(Query(connection, sql)) {
		for (idx_t i = 0; i < result->GetNames().size(); i++) {
			columns[result->GetNames()[i].GetIdentifierName()] = i;
		}
	}
	idx_t Size() const {
		return result->RowCount();
	}
	Value Get(const string &name, idx_t row) {
		auto entry = columns.find(name);
		if (entry == columns.end()) {
			throw InvalidInputException("DuckLake metadata is missing required column '%s'", name);
		}
		return result->GetValue(entry->second, row);
	}
	string Text(const string &name, idx_t row) {
		return Get(name, row).ToString();
	}
	int64_t Number(const string &name, idx_t row) {
		return Get(name, row).GetValue<int64_t>();
	}
	bool Visible(idx_t row, int64_t snapshot) {
		return Number("begin_snapshot", row) <= snapshot &&
		       (Get("end_snapshot", row).IsNull() || Number("end_snapshot", row) > snapshot);
	}
	optional<int64_t> OptionalNumber(const string &name, idx_t row) {
		auto value = Get(name, row);
		return value.IsNull() ? optional<int64_t>() : optional<int64_t>(value.GetValue<int64_t>());
	}

private:
	unique_ptr<ConversionQueryResult> result;
	map<string, idx_t> columns;
};

static unique_ptr<IcebergColumnDefinition> ReadColumn(MetadataRows &rows, idx_t row,
                                                      const map<int64_t, vector<idx_t>> &children) {
	auto column = make_uniq<IcebergColumnDefinition>();
	column->id = NumericCast<int32_t>(rows.Number("column_id", row));
	column->name = rows.Text("column_name", row);
	column->required = !rows.Get("nulls_allowed", row).GetValue<bool>();
	auto type = rows.Text("column_type", row);
	auto found = children.find(column->id);
	if (found != children.end()) {
		for (auto child_row : found->second) {
			auto child = ReadColumn(rows, child_row, children);
			if (type == "map" && column->GetChildCount() == 0) {
				child->required = true; // Both formats prohibit NULL map keys.
			}
			column->AddChild(std::move(child));
		}
	}
	if (type == "struct") {
		child_list_t<LogicalType> types;
		for (auto &child : column->GetChildren()) {
			types.emplace_back(child->name, child->type);
		}
		column->type = LogicalType::STRUCT(std::move(types));
	} else if (type == "list" && column->GetChildCount() == 1) {
		column->type = LogicalType::LIST(column->GetChild(0)->type);
	} else if (type == "map" && column->GetChildCount() == 2) {
		column->type = LogicalType::MAP(column->GetChild(0)->type, column->GetChild(1)->type);
	} else {
		column->type = IcebergType(type);
	}
	auto initial = rows.Get("initial_default", row);
	auto current = rows.Get("default_value", row);
	if (!initial.IsNull()) {
		column->initial_default = make_uniq<Value>(initial.DefaultCastAs(column->type));
	}
	if (!current.IsNull() && current.ToString() != "NULL") {
		if (rows.Text("default_value_type", row) == "expression") {
			auto expression = Parser::GetBuiltinParser().ParseSingleExpression(current.ToString());
			if (expression->GetExpressionClass() != ExpressionClass::CONSTANT) {
				throw InvalidInputException("Column %s has an expression default; Iceberg requires literal defaults",
				                            column->name);
			}
			current = expression->Cast<ConstantExpression>().GetLiteral().ToValue();
		} else if (rows.Text("default_value_type", row) != "literal") {
			throw InvalidInputException("Column %s has an unsupported default dialect/type", column->name);
		}
		column->write_default = make_uniq<Value>(current.DefaultCastAs(column->type));
	}
	return column;
}

static bool NeedsV3(const IcebergColumnDefinition &column) {
	if ((column.initial_default && !column.initial_default->IsNull()) ||
	    (column.write_default && !column.write_default->IsNull()) || column.type.id() == LogicalTypeId::VARIANT ||
	    column.type.id() == LogicalTypeId::TIMESTAMP_NS || column.type.id() == LogicalTypeId::TIMESTAMP_TZ_NS) {
		return true;
	}
	for (auto &child : column.GetChildren()) {
		if (NeedsV3(*child)) {
			return true;
		}
	}
	return false;
}

static shared_ptr<IcebergTableSchema> ReadSchema(MetadataRows &columns, int64_t snapshot, int32_t id) {
	map<int64_t, vector<idx_t>> children;
	auto result = make_shared_ptr<IcebergTableSchema>();
	result->schema_id = id;
	result->last_column_id = 0;
	for (idx_t i = 0; i < columns.Size(); i++) {
		if (!columns.Visible(i, snapshot)) {
			continue;
		}
		auto parent = columns.Get("parent_column", i);
		children[parent.IsNull() ? 0 : parent.GetValue<int64_t>()].push_back(i);
		result->last_column_id = MaxValue<idx_t>(result->last_column_id, columns.Number("column_id", i));
	}
	for (auto &entry : children) {
		std::sort(entry.second.begin(), entry.second.end(), [&](idx_t a, idx_t b) {
			return columns.Number("column_order", a) < columns.Number("column_order", b);
		});
	}
	for (auto row : children[0]) {
		result->columns.push_back(ReadColumn(columns, row, children));
	}
	VerifySchema(*result, false);
	return result;
}

static void CheckExternalFile(MetadataRows &rows, idx_t row, bool allow_partial) {
	auto encryption = rows.Get("encryption_key", row);
	if (!encryption.IsNull() && !encryption.ToString().empty()) {
		throw InvalidInputException("Encrypted DuckLake file %s cannot be exported without rewriting",
		                            rows.Text("path", row));
	}
	if (!allow_partial && !rows.Get("partial_max", row).IsNull()) {
		throw InvalidInputException("DuckLake file %s requires per-row snapshot filtering; retained history cannot "
		                            "be exported without rewriting",
		                            rows.Text("path", row));
	}
}

static void ReadTableFiles(ClientContext &context, Connection &connection, const DuckLakeMetadata &lake,
                           ConversionTable &table, int64_t table_id, const string &table_path) {
	auto predicate = " WHERE table_id = " + to_string(table_id);
	MetadataRows data(connection, "SELECT * FROM " + lake.prefix + "ducklake_data_file" + predicate);
	MetadataRows deletes(connection, "SELECT * FROM " + lake.prefix + "ducklake_delete_file" + predicate);
	MetadataRows partition_values(connection,
	                              "SELECT * FROM " + lake.prefix + "ducklake_file_partition_value" + predicate);
	map<int64_t, string> file_paths;
	for (idx_t i = 0; i < data.Size(); i++) {
		file_paths[data.Number("data_file_id", i)] =
		    ResolvePath(context, table_path, data.Text("path", i), data.Get("path_is_relative", i).GetValue<bool>());
	}
	for (auto &state : table.states) {
		for (idx_t i = 0; i < data.Size(); i++) {
			if (!data.Visible(i, state.source_id)) {
				continue;
			}
			CheckExternalFile(data, i, false);
			if (!data.Get("mapping_id", i).IsNull()) {
				throw InvalidInputException("%s: per-file name/field mappings require rewriting",
				                            table.QualifiedName());
			}
			ConversionFile file;
			file.schema_id = state.schema_id;
			file.spec_id =
			    data.Get("partition_id", i).IsNull() ? 0 : NumericCast<int32_t>(data.Number("partition_id", i) + 1);
			file.row_id_start = data.OptionalNumber("row_id_start", i);
			file.footer_size = data.OptionalNumber("footer_size", i);
			auto &entry = file.entry;
			entry.status = IcebergManifestEntryStatusType::ADDED;
			auto &output = entry.data_file;
			output.content = IcebergManifestEntryContentType::DATA;
			output.file_path = file_paths.at(data.Number("data_file_id", i));
			output.file_format = data.Text("file_format", i);
			output.record_count = data.Number("record_count", i);
			output.file_size_in_bytes = data.Number("file_size_bytes", i);
			auto &spec = table.metadata.partition_specs.at(file.spec_id);
			for (idx_t j = 0; j < spec.fields.size(); j++) {
				Value value;
				for (idx_t k = 0; k < partition_values.Size(); k++) {
					if (partition_values.Number("data_file_id", k) == data.Number("data_file_id", i) &&
					    partition_values.Number("partition_key_index", k) == NumericCast<int64_t>(j)) {
						auto &field = spec.fields[j];
						auto column = table.metadata.FindColumnByFieldId(field.source_id);
						value = partition_values.Get("partition_value", k);
						if (!value.IsNull()) {
							value = value.DefaultCastAs(field.transform.GetSerializedType(column->type));
							if (table.calendar_year_fields.count(field.partition_field_id)) {
								value = Value::INTEGER(NumericCast<int32_t>(value.GetValue<int64_t>() - 1970));
							}
						}
					}
				}
				output.partition_info.push_back({spec.fields[j].partition_field_id, value});
			}
			state.files.push_back(std::move(file));
		}
		for (idx_t i = 0; i < deletes.Size(); i++) {
			if (!deletes.Visible(i, state.source_id)) {
				continue;
			}
			bool puffin = StringUtil::CIEquals(deletes.Text("format", i), "puffin");
			CheckExternalFile(deletes, i, puffin);
			ConversionFile file;
			file.schema_id = state.schema_id;
			file.footer_size = deletes.OptionalNumber("footer_size", i);
			auto &entry = file.entry;
			entry.status = IcebergManifestEntryStatusType::ADDED;
			auto &output = entry.data_file;
			output.content = IcebergManifestEntryContentType::POSITION_DELETES;
			output.file_format = deletes.Text("format", i);
			output.file_path = ResolvePath(context, table_path, deletes.Text("path", i),
			                               deletes.Get("path_is_relative", i).GetValue<bool>());
			output.record_count = deletes.Number("delete_count", i);
			output.file_size_in_bytes = deletes.Number("file_size_bytes", i);
			output.referenced_data_file = file_paths.at(deletes.Number("data_file_id", i));
			for (auto &data_file : state.files) {
				if (data_file.entry.data_file.file_path == *output.referenced_data_file) {
					file.spec_id = data_file.spec_id;
					output.partition_info = data_file.entry.data_file.partition_info;
					break;
				}
			}
			output.lower_bounds[2147483546] = Value::BLOB(*output.referenced_data_file);
			output.upper_bounds[2147483546] = Value::BLOB(*output.referenced_data_file);
			if (puffin) {
				table.metadata.iceberg_version = 3;
				if (file.footer_size && *file.footer_size > 0) {
					auto footer = IcebergPuffinReader::ReadFooter(FileSystem::GetFileSystem(context), output.file_path,
					                                              output.file_size_in_bytes, file.footer_size);
					if (std::holds_alternative<string>(footer)) {
						throw InvalidInputException("%s: %s", table.QualifiedName(), std::get<string>(footer));
					}
					auto &blobs = std::get<IcebergPuffinFileFooter>(footer).file_metadata.blobs;
					int64_t latest = -1;
					for (auto &blob : blobs) {
						if (blob.type != "deletion-vector-v1" || blob.compression_codec || !blob.properties) {
							throw InvalidInputException("Unsupported Puffin blob in %s", output.file_path);
						}
						auto version = blob.properties->find("ducklake-snapshot-id");
						auto id = version == blob.properties->end() ? deletes.Number("begin_snapshot", i)
						                                            : Value(version->second).GetValue<int64_t>();
						if (id <= state.source_id && id > latest) {
							output.content_offset = blob.offset;
							output.content_size_in_bytes = blob.length;
							auto count = blob.properties->find("cardinality");
							if (count == blob.properties->end()) {
								throw InvalidInputException("Puffin blob in %s is missing cardinality",
								                            output.file_path);
							}
							output.record_count = Value(count->second).GetValue<int64_t>();
							latest = id;
						}
					}
					if (latest < 0) {
						continue;
					}
				} else {
					throw InvalidInputException("%s: bare DuckLake deletion vector %s has no Puffin container; "
					                            "Iceberg requires a container, which cannot be added without rewriting",
					                            table.QualifiedName(), output.file_path);
				}
			}
			state.files.push_back(std::move(file));
		}
	}
}

ConversionPlan ReadDuckLake(ClientContext &context, Connection &connection, const ConversionOptions &options,
                            const DuckLakeMetadata &lake) {
	MetadataRows tables(connection, "SELECT * FROM " + lake.prefix + "ducklake_table WHERE end_snapshot IS NULL");
	MetadataRows schemas(connection, "SELECT * FROM " + lake.prefix + "ducklake_schema");
	MetadataRows snapshots(connection, "SELECT * FROM " + lake.prefix + "ducklake_snapshot ORDER BY snapshot_id");
	ConversionPlan result;
	map<int64_t, idx_t> schema_rows;
	for (idx_t i = 0; i < schemas.Size(); i++) {
		if (schemas.Get("end_snapshot", i).IsNull()) {
			schema_rows[schemas.Number("schema_id", i)] = i;
		}
	}
	auto resolve_schema = [&](auto &&self, int64_t schema_id,
	                          unordered_set<int64_t> &seen) -> pair<vector<string>, string> {
		auto entry = schema_rows.find(schema_id);
		if (entry == schema_rows.end() || !seen.insert(schema_id).second) {
			throw InvalidInputException("DuckLake metadata contains a missing or cyclic schema parent");
		}
		auto row = entry->second;
		pair<vector<string>, string> resolved {{}, lake.data_path};
		if (lake.v11 && !schemas.Get("parent_schema_id", row).IsNull()) {
			resolved = self(self, schemas.Number("parent_schema_id", row), seen);
		}
		resolved.first.push_back(schemas.Text("schema_name", row));
		// Namespace ancestry does not change the storage base: schema paths are catalog-relative.
		resolved.second = ResolvePath(context, lake.data_path, schemas.Text("path", row),
		                              schemas.Get("path_is_relative", row).GetValue<bool>());
		return resolved;
	};
	if (!options.tables) {
		for (auto &schema : schema_rows) {
			unordered_set<int64_t> seen;
			result.namespaces.push_back(resolve_schema(resolve_schema, schema.first, seen).first);
		}
	}
	for (idx_t i = 0; i < tables.Size(); i++) {
		auto schema_id = tables.Number("schema_id", i);
		unordered_set<int64_t> seen;
		auto resolved_schema = resolve_schema(resolve_schema, schema_id, seen);
		auto &namespace_items = resolved_schema.first;
		auto &schema_path = resolved_schema.second;
		auto name = tables.Text("table_name", i);
		if (!Selected(options, namespace_items, name)) {
			continue;
		}
		auto table = make_uniq<ConversionTable>();
		table->namespace_items = namespace_items;
		table->name = name;
		table->metadata.table_uuid = tables.Text("table_uuid", i);
		table->source_uuid = table->metadata.table_uuid;
		table->metadata.iceberg_version = 2;
		auto table_id = tables.Number("table_id", i);
		auto predicate = " WHERE table_id = " + to_string(table_id);
		auto creation =
		    Query(connection, "SELECT min(begin_snapshot) FROM " + lake.prefix + "ducklake_table" + predicate);
		auto begin_snapshot = creation->GetValue(0, 0).GetValue<int64_t>();
		MetadataRows columns(connection, "SELECT * FROM " + lake.prefix + "ducklake_column" + predicate);
		MetadataRows partitions(connection, "SELECT * FROM " + lake.prefix + "ducklake_partition_info" + predicate);
		MetadataRows fields(connection, "SELECT * FROM " + lake.prefix + "ducklake_partition_column" + predicate +
		                                    " ORDER BY partition_key_index");
		MetadataRows inlined(connection, "SELECT * FROM " + lake.prefix + "ducklake_inlined_data_tables" + predicate);
		for (idx_t j = 0; j < inlined.Size(); j++) {
			auto count =
			    Query(connection, "SELECT count(*) FROM " + lake.prefix + IdentifierSQL(inlined.Text("table_name", j)));
			if (count->GetValue(0, 0).GetValue<int64_t>() != 0) {
				throw InvalidInputException("%s: retained inline rows require rewriting before conversion",
				                            table->QualifiedName());
			}
		}
		auto inline_deletes = "ducklake_inlined_delete_" + to_string(table_id);
		if (connection.TableInfo(Identifier(lake.catalog), Identifier(lake.schema), Identifier(inline_deletes))) {
			auto count = Query(connection, "SELECT count(*) FROM " + lake.prefix + IdentifierSQL(inline_deletes));
			if (count->GetValue(0, 0).GetValue<int64_t>() != 0) {
				throw InvalidInputException("%s: inline deletes require rewriting before conversion",
				                            table->QualifiedName());
			}
		}
		shared_ptr<IcebergTableSchema> previous;
		int32_t next_schema = 0;
		idx_t last_column = 0;
		for (idx_t j = 0; j < snapshots.Size(); j++) {
			auto id = snapshots.Number("snapshot_id", j);
			if (id < begin_snapshot) {
				continue;
			}
			auto schema = ReadSchema(columns, id, next_schema);
			if (!previous || !previous->Equals(*schema)) {
				next_schema++;
				previous = schema;
				table->metadata.GetSchemasMutable().AddSchemaOrGetExisting(schema);
			}
			last_column = MaxValue(last_column, schema->last_column_id);
			for (auto &column : schema->columns) {
				if (NeedsV3(*column)) {
					table->metadata.iceberg_version = 3;
				}
			}
			ConversionState state;
			state.source_id = id;
			state.timestamp = snapshots.Get("snapshot_time", j).GetValue<timestamp_t>();
			state.schema_id = previous->schema_id;
			for (idx_t k = 0; k < partitions.Size(); k++) {
				if (partitions.Visible(k, id)) {
					state.spec_id = NumericCast<int32_t>(partitions.Number("partition_id", k) + 1);
				}
			}
			table->states.push_back(std::move(state));
		}
		if (table->states.empty()) {
			throw InvalidInputException("%s has no retained DuckLake snapshots", table->QualifiedName());
		}
		table->metadata.last_column_id = last_column;
		table->metadata.SetCurrentSchemaId(table->states.back().schema_id);
		table->metadata.partition_specs.emplace(0, IcebergPartitionSpec(0));
		idx_t next_partition_field = 1000;
		for (idx_t j = 0; j < partitions.Size(); j++) {
			auto source_id = partitions.Number("partition_id", j);
			IcebergPartitionSpec spec(NumericCast<int32_t>(source_id + 1));
			if (table->metadata.partition_specs.count(spec.spec_id)) {
				continue;
			}
			for (idx_t k = 0; k < fields.Size(); k++) {
				if (fields.Number("partition_id", k) != source_id) {
					continue;
				}
				IcebergPartitionSpecField field;
				field.source_id = fields.Number("column_id", k);
				field.partition_field_id = next_partition_field++;
				field.transform = IcebergPartitionTransform(fields.Text("transform", k));
				if (fields.Text("transform", k) == "year") {
					table->calendar_year_fields.insert(field.partition_field_id);
				}
				auto column = table->metadata.FindColumnByFieldId(field.source_id);
				if (!column) {
					throw InvalidInputException("%s: partition field references a missing column",
					                            table->QualifiedName());
				}
				field.SetPartitionSpecFieldName(column->name);
				spec.fields.push_back(std::move(field));
			}
			table->metadata.partition_specs.emplace(spec.spec_id, std::move(spec));
		}
		table->metadata.last_partition_field_id = next_partition_field - 1;
		table->metadata.default_spec_id = table->states.back().spec_id;
		auto path = ResolvePath(context, schema_path, tables.Text("path", i),
		                        tables.Get("path_is_relative", i).GetValue<bool>());
		ReadTableFiles(context, connection, lake, *table, table_id, path);
		result.tables.push_back(std::move(table));
	}
	return result;
}

static string WriteImportedHistory(ClientContext &context, ConversionTable &table, vector<string> &created_files) {
	auto &metadata = table.metadata;
	auto &fs = FileSystem::GetFileSystem(context);
	auto &copy = IcebergUtils::GetCopyFunction(context, "avro").function;
	auto &db = DatabaseInstance::GetDatabase(context);
	auto metadata_path = metadata.GetMetadataPath(fs);
	fs.CreateDirectoriesRecursive(metadata_path);
	JSONWriter writer;
	auto root = writer.CreateObject();
	auto requirements = writer.CreateArray();
	auto requirement = writer.CreateObject();
	requirement.AddString("type", "assert-create");
	requirements.Append(requirement);
	root.Add("requirements", requirements);
	auto updates = writer.CreateArray();
	root.Add("updates", updates);
	auto update = [&](const string &action) {
		auto value = writer.CreateObject();
		value.AddString("action", action);
		updates.Append(value);
		return value;
	};
	update("assign-uuid").AddString("uuid", metadata.table_uuid);
	update("upgrade-format-version").Add("format-version", writer.CreateSignedInteger(metadata.iceberg_version));
	map<int32_t, reference<const IcebergTableSchema>> ordered_schemas;
	metadata.GetSchemas().ForEachSchema(
	    [&](const IcebergTableSchema &schema) { ordered_schemas.emplace(schema.schema_id, schema); });
	for (auto &item : ordered_schemas) {
		auto &schema = item.second.get();
		if (schema.schema_id == 0) {
			continue;
		}
		auto value = update("add-schema");
		value.Add("schema", schema.ToRESTObject().ToJSON(writer));
		value.Add("last-column-id", writer.CreateUnsignedInteger(metadata.last_column_id.GetIndex()));
	}
	map<int32_t, reference<const IcebergPartitionSpec>> ordered_specs;
	for (auto &spec : metadata.partition_specs) {
		ordered_specs.emplace(spec.first, spec.second);
	}
	for (auto &spec : ordered_specs) {
		if (spec.first != 0) {
			update("add-spec").Add("spec", spec.second.get().ToJSON(writer));
		}
	}
	map<string, pair<int64_t, int64_t>> file_origins;
	map<string, IcebergManifestEntry> previous_files;
	int64_t next_row_id = 0;
	optional<int64_t> parent;
	for (auto &state : table.states) {
		IcebergSnapshot snapshot(state.schema_id, state.source_id);
		snapshot.sequence_number = state.source_id;
		snapshot.parent_snapshot_id = parent;
		snapshot.timestamp_ms = timestamp_ms_t(Timestamp::GetEpochMs(state.timestamp));
		snapshot.operation = parent ? IcebergSnapshotOperationType::OVERWRITE : IcebergSnapshotOperationType::APPEND;
		snapshot.manifest_list = fs.JoinPath(metadata_path, UUID::ToString(UUID::GenerateRandomUUID()) + "-snap.avro");
		auto row_start = next_row_id;
		IcebergManifestList list(snapshot.manifest_list);
		map<pair<int32_t, int32_t>, vector<IcebergManifestEntry>> grouped;
		int64_t data_count = 0, delete_count = 0, records = 0, deleted = 0, bytes = 0;
		case_insensitive_map_t<string> metrics;
		map<string, int64_t> changes;
		unordered_set<string> current_files;
		auto account = [&](const IcebergManifestEntry &entry, bool added) {
			auto &data = entry.data_file;
			if (data.content == IcebergManifestEntryContentType::DATA) {
				changes[added ? "added-data-files" : "deleted-data-files"]++;
				changes[added ? "added-records" : "deleted-records"] += data.record_count;
			} else {
				changes[added ? "added-delete-files" : "removed-delete-files"]++;
				changes[added ? "added-position-deletes" : "removed-position-deletes"] += data.record_count;
			}
			changes[added ? "added-files-size" : "removed-files-size"] += data.GetContentSizeInBytes();
		};
		for (auto &file : state.files) {
			auto entry = file.entry;
			entry.data_file.file_format = StringUtil::Upper(entry.data_file.file_format);
			auto identity = file.Identity();
			current_files.insert(identity);
			auto origin = file_origins.find(identity);
			bool added = !previous_files.count(identity);
			if (added) {
				file_origins.erase(identity);
				origin = file_origins.emplace(identity, make_pair(state.source_id, next_row_id)).first;
				if (entry.data_file.content == IcebergManifestEntryContentType::DATA) {
					next_row_id += entry.data_file.record_count;
				}
				account(entry, true);
			}
			entry.status = added ? IcebergManifestEntryStatusType::ADDED : IcebergManifestEntryStatusType::EXISTING;
			entry.SetSnapshotId(origin->second.first);
			entry.SetSequenceNumber(origin->second.first);
			entry.SetFileSequenceNumber(origin->second.first);
			bytes += entry.data_file.GetContentSizeInBytes();
			if (entry.data_file.content == IcebergManifestEntryContentType::DATA) {
				if (metadata.iceberg_version == 3) {
					entry.data_file.SetFirstRowId(origin->second.second);
				}
				data_count++;
				records += entry.data_file.record_count;
			} else {
				delete_count++;
				deleted += entry.data_file.record_count;
			}
			grouped[{file.spec_id, static_cast<int32_t>(entry.data_file.content)}].push_back(std::move(entry));
		}
		for (auto &previous : previous_files) {
			if (!current_files.count(previous.first)) {
				account(previous.second, false);
			}
		}
		bool inserted = changes["added-data-files"] > 0;
		bool removed = changes["deleted-data-files"] > 0 || changes["added-delete-files"] > 0 ||
		               changes["removed-delete-files"] > 0;
		snapshot.operation =
		    inserted ? (removed ? IcebergSnapshotOperationType::OVERWRITE : IcebergSnapshotOperationType::APPEND)
		             : (removed ? IcebergSnapshotOperationType::DELETE : IcebergSnapshotOperationType::REPLACE);
		for (auto &group : grouped) {
			auto content =
			    group.first.second == 0 ? IcebergManifestContentType::DATA : IcebergManifestContentType::DELETE;
			IcebergManifestMetadata manifest_metadata(state.schema_id, group.first.first, metadata.iceberg_version,
			                                          content);
			optional<int64_t> first_row_id;
			if (metadata.iceberg_version == 3 && content == IcebergManifestContentType::DATA) {
				first_row_id = row_start;
				for (auto &entry : group.second) {
					if (entry.status == IcebergManifestEntryStatusType::ADDED) {
						first_row_id = MinValue(*first_row_id, entry.data_file.GetFirstRowId());
					}
				}
			}
			auto manifest = IcebergManifestListEntry::CreateFromEntries(state.source_id, metadata, manifest_metadata,
			                                                            std::move(group.second), first_row_id);
			auto path = fs.JoinPath(metadata_path, UUID::ToString(UUID::GenerateRandomUUID()) + "-m.avro");
			created_files.push_back(path);
			auto length = manifest_file::WriteToFile(metadata, manifest_metadata, manifest.GetManifestEntries(), path,
			                                         copy, db, context);
			list.AddExistingManifestFile(
			    IcebergManifestListEntry::CreateWritten(std::move(manifest), path, length, state.source_id));
		}
		created_files.push_back(snapshot.manifest_list);
		manifest_list::WriteToFile(metadata, list, copy, db, context);
		if (metadata.iceberg_version == 3) {
			snapshot.first_row_id = row_start;
			snapshot.added_rows = next_row_id - row_start;
		}
		metrics = {{"total-data-files", to_string(data_count)},
		           {"total-delete-files", to_string(delete_count)},
		           {"total-records", to_string(records)},
		           {"total-position-deletes", to_string(deleted)},
		           {"total-files-size", to_string(bytes)}};
		for (auto &change : changes) {
			metrics[change.first] = to_string(change.second);
		}
		snapshot.metrics = IcebergSnapshotMetrics(metrics);
		auto serialized = snapshot.ToRESTObject(metadata);
		serialized.summary.additional_properties["ducklake.snapshot-id"] = to_string(state.source_id);
		serialized.summary.additional_properties["ducklake.timestamp-us"] = to_string(state.timestamp.value);
		serialized.summary.additional_properties["ducklake.table-uuid"] = table.source_uuid;
		update("add-snapshot").Add("snapshot", serialized.ToJSON(writer));
		auto reference = update("set-snapshot-ref");
		reference.AddString("ref-name", "main");
		reference.AddString("type", "branch");
		reference.Add("snapshot-id", writer.CreateSignedInteger(state.source_id));
		parent = state.source_id;
		previous_files.clear();
		for (auto &file : state.files) {
			previous_files.emplace(file.Identity(), file.entry);
		}
	}
	update("set-current-schema").Add("schema-id", writer.CreateSignedInteger(metadata.GetCurrentSchemaId()));
	update("set-default-spec").Add("spec-id", writer.CreateSignedInteger(metadata.default_spec_id));
	writer.SetRoot(root);
	return writer.ToString();
}

void WriteIceberg(ClientContext &context, Connection &connection, const ConversionOptions &options,
                  const ConversionPlan &plan) {
	auto &tables = plan.tables;
	auto &base = Catalog::GetCatalog(context, Identifier(options.destination));
	if (base.GetCatalogType() != "iceberg") {
		throw InvalidInputException("Destination must be an attached Iceberg REST catalog");
	}
	auto &catalog = base.Cast<IcebergCatalog>();
	if (!tables.empty() && !catalog.attach_options.stage_create_tables) {
		throw InvalidInputException(
		    "ducklake_to_iceberg requires STAGE_CREATE_TABLES true for atomic table publication");
	}
	// Check every selected name before staging even the first table.
	for (auto &parts : plan.namespaces) {
		for (auto &part : parts) {
			if (part.find('.') != string::npos) {
				throw InvalidInputException(
				    "The Iceberg catalog cannot distinguish namespace components containing '.'");
			}
		}
	}
	for (auto &table : tables) {
		for (auto &part : table->namespace_items) {
			if (part.find('.') != string::npos) {
				throw InvalidInputException("%s: Iceberg cannot distinguish namespace components containing '.'",
				                            table->QualifiedName());
			}
		}
		auto existing = IcebergLoadTableRequest(table->namespace_items, table->name).Execute(context, catalog);
		if (existing.status_ == HTTPStatusCode::OK_200) {
			throw InvalidInputException("Destination table %s already exists; conversion never overwrites tables",
			                            table->QualifiedName());
		}
		if (existing.status_ != HTTPStatusCode::NotFound_404) {
			throw InvalidInputException("Cannot verify absence of destination table %s (HTTP %d)",
			                            table->QualifiedName(), static_cast<int>(existing.status_));
		}
	}
	vector<string> completed;
	Connection namespace_connection(DatabaseInstance::GetDatabase(context));
	unordered_set<string> ensured_namespaces;
	auto ensure_namespace = [&](const vector<string> &parts) {
		vector<string> path;
		for (auto &part : parts) {
			path.push_back(part);
			auto sql_path = QualifiedSQL({options.destination, StringUtil::Join(path, ".")});
			if (ensured_namespaces.insert(sql_path).second) {
				Query(namespace_connection, "CREATE SCHEMA IF NOT EXISTS " + sql_path);
			}
		}
	};
	for (auto &parts : plan.namespaces) {
		ensure_namespace(parts);
	}
	for (auto &table : tables) {
		vector<string> created_files;
		bool publication_started = false;
		bool definitely_failed = false;
		try {
			ensure_namespace(table->namespace_items);
			auto first_schema = table->metadata.GetSchemaFromId(table->states.front().schema_id).Copy();
			IcebergCreateTableRequest request(table->name, first_schema, IcebergPartitionSpec(0), IcebergSortOrder(0),
			                                  table->metadata.iceberg_version, {}, "");
			auto staged = IRCAPI::CommitNewTable(context, catalog, table->namespace_items, request);
			auto staged_metadata = IcebergTableMetadata::FromTableMetadata(staged.metadata);
			if (!staged_metadata.GetLatestSchema().Equals(*first_schema)) {
				throw InvalidInputException("Catalog reassigned field IDs while staging %s; files cannot be reused",
				                            table->QualifiedName());
			}
			table->metadata.location = staged_metadata.location;
			table->metadata.table_uuid = staged_metadata.table_uuid;
			auto schema_entry = catalog.GetSchemas().GetEntry(context, StringUtil::Join(table->namespace_items, "."),
			                                                  OnEntryNotFound::THROW_EXCEPTION);
			IcebergTable staged_table(catalog, schema_entry->Cast<IcebergSchemaEntry>(), table->name, staged);
			staged_table.LoadCredentials(context);
			auto body = WriteImportedHistory(context, *table, created_files);
			publication_started = true;
			auto commit = IRCAPI::CommitTableUpdate(context, catalog, table->namespace_items, table->name, body);
			if (!commit.Success()) {
				auto status = static_cast<int>(commit.status);
				definitely_failed = status >= 400 && status < 500;
				commit.Throw(catalog.GetBaseUrl().GetURLEncoded());
			}
			completed.push_back(table->QualifiedName());
		} catch (std::exception &ex) {
			if (!publication_started || definitely_failed) {
				auto &fs = FileSystem::GetFileSystem(context);
				for (auto &path : created_files) {
					try {
						fs.TryRemoveFile(path);
					} catch (...) {
					}
				}
			}
			throw InvalidInputException("ducklake_to_iceberg failed at %s: %s. Completed tables: [%s]. "
			                            "Exclude completed tables when retrying. %s",
			                            table->QualifiedName(), ex.what(), StringUtil::Join(completed, ", "),
			                            publication_started && !definitely_failed
			                                ? "Commit outcome may be unknown; inspect the destination before retrying."
			                                : "This table was not published.");
		}
	}
}

} // namespace conversion
} // namespace iceberg

TableFunctionSet IcebergFunctions::GetDuckLakeToIcebergFunction() {
	return iceberg::conversion::ConversionFunction(false);
} // namespace duckdb
} // namespace duckdb
