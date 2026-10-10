#include "function/ducklake/catalog_conversion.hpp"
#include "function/iceberg_functions.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/iceberg_schema_set.hpp"
#include "catalog/rest/iceberg_table_set.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table.hpp"
#include "duckdb/common/json_document.hpp"
#include "duckdb/common/types/uuid.hpp"
#include "duckdb/common/operator/cast_operators.hpp"
#include "core/metadata/puffin/iceberg_puffin_metadata.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"

namespace duckdb {
namespace iceberg {
namespace conversion {

static string Provenance(const string &uuid, int64_t snapshot, timestamp_t timestamp, bool synthetic) {
	JSONWriter writer;
	auto root = writer.CreateObject();
	root.AddString("source-format", "iceberg");
	root.AddString("table-uuid", uuid);
	root.Add("snapshot-id", writer.CreateSignedInteger(snapshot));
	root.AddString("source-timestamp", Timestamp::ToString(timestamp) + "Z");
	root.Add("synthetic", writer.CreateBoolean(synthetic));
	writer.SetRoot(root);
	return writer.ToString();
}

static void ValidatePuffinImport(ClientContext &context, ConversionFile &file) {
	auto &data = file.entry.data_file;
	if (data.content_offset && data.content_size_in_bytes && *data.content_offset == 0 &&
	    *data.content_size_in_bytes == data.file_size_in_bytes) {
		file.footer_size = 0;
		return;
	}
	auto &fs = FileSystem::GetFileSystem(context);
	auto footer = IcebergPuffinReader::ReadFooter(fs, data.file_path, data.file_size_in_bytes);
	if (std::holds_alternative<string>(footer)) {
		throw InvalidInputException("Cannot import deletion vector %s: %s", data.file_path, std::get<string>(footer));
	}
	auto &parsed = std::get<IcebergPuffinFileFooter>(footer);
	if (parsed.file_metadata.blobs.size() != 1) {
		throw InvalidInputException("Puffin file %s contains multiple blobs; DuckLake cannot select an Iceberg "
		                            "blob by offset without rewriting",
		                            data.file_path);
	}
	auto &blob = parsed.file_metadata.blobs[0];
	if (blob.type != "deletion-vector-v1" || blob.compression_codec || !data.content_offset ||
	    blob.offset != *data.content_offset || !data.content_size_in_bytes ||
	    blob.length != *data.content_size_in_bytes) {
		throw InvalidInputException("Puffin file %s has an incompatible deletion-vector layout", data.file_path);
	}
	if (blob.properties && blob.properties->count("ducklake-snapshot-id")) {
		throw InvalidInputException("Puffin file %s embeds source DuckLake snapshot IDs that need remapping",
		                            data.file_path);
	}
	file.footer_size = parsed.footer_size;
}

static ConversionState ReadState(ClientContext &context, IcebergTableMetadata &metadata,
                                 const IcebergSnapshot &snapshot) {
	ConversionState state;
	state.source_id = snapshot.snapshot_id;
	state.timestamp = Cast::Operation<timestamp_ms_t, timestamp_t>(snapshot.timestamp_ms);
	state.schema_id = snapshot.GetSchemaId();
	state.spec_id = metadata.default_spec_id;
	state.provenance = Provenance(metadata.table_uuid, state.source_id, state.timestamp, false);
	IcebergSnapshotScanInfo scan_info;
	scan_info.snapshot = snapshot;
	scan_info.schema_id = snapshot.GetSchemaId();
	IcebergOptions options;
	auto manifests = IcebergManifestList::Load(metadata.location, metadata, scan_info, context, options);
	for (auto &manifest : manifests->GetManifestFilesConst()) {
		for (auto &entry : manifest.GetManifestEntries()) {
			if (entry.status == IcebergManifestEntryStatusType::DELETED) {
				continue;
			}
			ConversionFile file;
			file.entry = entry;
			file.spec_id = manifest.GetFile().partition_spec_id;
			file.schema_id = snapshot.GetSchemaId();
			file.entry.data_file.file_path = ResolvePath(context, metadata.location, entry.data_file.file_path, false);
			if (file.entry.data_file.content == IcebergManifestEntryContentType::POSITION_DELETES) {
				file.entry.data_file.referenced_data_file =
				    ResolvePath(context, metadata.location, ReferencedDataFile(file), false);
			}
			file.entry.SetSequenceNumber(entry.GetSequenceNumber(manifest.GetFile()));
			file.entry.SetFileSequenceNumber(entry.GetFileSequenceNumber(manifest.GetFile()));
			state.files.push_back(std::move(file));
		}
	}
	// An Iceberg delete can outlive the data it affected; it is not visible in that snapshot.
	unordered_map<string, int64_t> data_paths;
	for (auto &file : state.files) {
		if (file.entry.data_file.content == IcebergManifestEntryContentType::DATA) {
			data_paths[file.entry.data_file.file_path] = file.entry.ExplicitSequenceNumber().value_or(0);
		}
	}
	unordered_set<string> vector_paths;
	for (auto &file : state.files) {
		if (file.entry.data_file.IsDeletionVector()) {
			auto reference = ReferencedDataFile(file);
			auto data = data_paths.find(reference);
			if (data != data_paths.end() && data->second <= file.entry.ExplicitSequenceNumber().value_or(0)) {
				vector_paths.insert(reference);
			}
		}
	}
	vector<ConversionFile> live;
	for (auto &file : state.files) {
		if (file.entry.data_file.content == IcebergManifestEntryContentType::POSITION_DELETES) {
			auto reference = ReferencedDataFile(file);
			auto data = data_paths.find(reference);
			if (data == data_paths.end() || data->second > file.entry.ExplicitSequenceNumber().value_or(0) ||
			    (!file.entry.data_file.IsDeletionVector() && vector_paths.count(reference))) {
				continue;
			}
			if (file.entry.data_file.IsDeletionVector()) {
				ValidatePuffinImport(context, file);
			}
		}
		live.push_back(std::move(file));
	}
	state.files = std::move(live);
	return state;
}

static bool SameState(const ConversionState &a, const ConversionState &b) {
	if (a.schema_id != b.schema_id || a.spec_id != b.spec_id || a.files.size() != b.files.size()) {
		return false;
	}
	unordered_set<string> files;
	for (auto &file : a.files) {
		files.insert(file.Identity());
	}
	for (auto &file : b.files) {
		if (!files.count(file.Identity())) {
			return false;
		}
	}
	return true;
}

static void ReadHistory(ClientContext &context, ConversionTable &table) {
	auto &metadata = table.metadata;
	vector<IcebergTableMetadata> versions;
	auto &fs = FileSystem::GetFileSystem(context);
	for (auto &log : metadata.metadata_log) {
		auto raw = IcebergTableMetadata::Parse(log.metadata_file, fs,
		                                       StringUtil::EndsWith(log.metadata_file, ".gz") ? "gzip" : "none");
		auto historical = IcebergTableMetadata::FromTableMetadata(raw);
		if (historical.table_uuid != metadata.table_uuid) {
			throw InvalidInputException("%s has a metadata log for a different table", table.QualifiedName());
		}
		versions.push_back(std::move(historical));
	}
	versions.push_back(metadata.Copy());
	unordered_map<int32_t, shared_ptr<IcebergTableSchema>> schemas;
	for (auto &version : versions) {
		version.GetSchemas().ForEachSchema([&](const IcebergTableSchema &schema) {
			VerifySchema(schema, true);
			auto existing = schemas.find(schema.schema_id);
			if (existing != schemas.end() && !existing->second->Equals(schema)) {
				throw InvalidInputException("%s reuses schema ID %d for different schemas", table.QualifiedName(),
				                            schema.schema_id);
			}
			schemas.emplace(schema.schema_id, schema.Copy());
		});
		for (auto &spec : version.GetPartitionSpecs()) {
			for (auto &field : spec.second.fields) {
				DuckLakeTransform(field.transform);
			}
			metadata.partition_specs.emplace(spec.first, spec.second);
		}
	}
	metadata.GetSchemasMutable() = IcebergTableMetadataSchemas(std::move(schemas));
	vector<reference<const IcebergSnapshot>> chain;
	unordered_set<int64_t> seen;
	auto current = metadata.GetLatestSnapshot();
	while (current) {
		if (!seen.insert(current->snapshot_id).second) {
			throw InvalidInputException("%s has a cyclic snapshot history", table.QualifiedName());
		}
		chain.push_back(*current);
		auto parent = current->parent_snapshot_id ? metadata.snapshots.find(*current->parent_snapshot_id)
		                                          : metadata.snapshots.end();
		current = parent == metadata.snapshots.end() ? nullptr : optional_ptr<const IcebergSnapshot>(parent->second);
	}
	if (seen.size() != metadata.snapshots.size()) {
		throw InvalidInputException("%s has branching or rolled-back history; linear DuckLake history cannot "
		                            "preserve all retained states",
		                            table.QualifiedName());
	}
	unordered_set<int64_t> logged;
	for (auto &entry : metadata.snapshot_log) {
		if (seen.count(entry.first) && !logged.insert(entry.first).second) {
			throw InvalidInputException("%s has rollback events in its snapshot log", table.QualifiedName());
		}
	}
	std::reverse(chain.begin(), chain.end());
	struct Event {
		ConversionState state;
		idx_t rank;
		idx_t metadata_order;
	};
	vector<Event> events;
	map<int64_t, pair<ConversionState, idx_t>> snapshots;
	for (idx_t i = 0; i < chain.size(); i++) {
		auto state = ReadState(context, metadata, chain[i].get());
		for (auto &version : versions) {
			if (version.current_snapshot_id && *version.current_snapshot_id == state.source_id) {
				state.spec_id = version.default_spec_id;
				break;
			}
		}
		snapshots.emplace(state.source_id, make_pair(state, i + 1));
		events.push_back({std::move(state), i + 1, 0});
	}
	for (idx_t i = 0; i < versions.size(); i++) {
		auto &version = versions[i];
		ConversionState state;
		idx_t rank = 0;
		state.timestamp = Cast::Operation<timestamp_ms_t, timestamp_t>(version.last_updated_ms);
		if (version.current_snapshot_id) {
			auto snapshot = snapshots.find(*version.current_snapshot_id);
			if (snapshot == snapshots.end()) {
				continue; // Metadata for an expired snapshot is not retained table history.
			}
			state.files = snapshot->second.first.files;
			rank = snapshot->second.second;
		} else if (!chain.empty() && version.last_updated_ms > chain.front().get().timestamp_ms) {
			continue; // A staged bootstrap must not erase imported, backdated snapshots.
		}
		state.schema_id = version.GetCurrentSchemaId();
		state.spec_id = version.default_spec_id;
		state.synthetic = true;
		state.provenance = Provenance(metadata.table_uuid, 0, state.timestamp, true);
		events.push_back({std::move(state), rank, i + 1});
	}
	std::stable_sort(events.begin(), events.end(), [](const Event &a, const Event &b) {
		if (a.state.timestamp != b.state.timestamp) {
			return a.state.timestamp < b.state.timestamp;
		}
		if (a.rank != b.rank) {
			return a.rank < b.rank;
		}
		return a.metadata_order < b.metadata_order;
	});
	for (auto &event : events) {
		if (event.state.synthetic && !table.states.empty() && SameState(table.states.back(), event.state)) {
			continue;
		}
		table.states.push_back(std::move(event.state));
	}
}

ConversionPlan ReadIceberg(ClientContext &context, Connection &connection, const ConversionOptions &options) {
	auto &catalog = Catalog::GetCatalog(context, Identifier(options.source));
	if (catalog.GetCatalogType() != "iceberg") {
		throw InvalidInputException("Source must be an attached Iceberg catalog");
	}
	auto &iceberg = catalog.Cast<IcebergCatalog>();
	ConversionPlan result;
	struct NamespaceSelection {
		vector<string> parts;
		set<string> tables;
	};
	map<string, NamespaceSelection> selected;
	if (options.tables) {
		for (auto &name : *options.tables) {
			vector<string> parts;
			for (idx_t i = 0; i + 1 < name.Path().size(); i++) {
				auto components = IRCAPI::ParseSchemaName(name.Path()[i].GetIdentifierName());
				parts.insert(parts.end(), components.begin(), components.end());
			}
			if (!Selected(options, parts, name.Name().GetIdentifierName())) {
				continue;
			}
			auto &entry = selected[QualifiedSQL(parts)];
			entry.parts = parts;
			entry.tables.insert(name.Name().GetIdentifierName());
		}
	} else {
		auto schemas = IcebergListSchemasRequest({}).Execute(context, iceberg);
		for (auto &schema : schemas) {
			result.namespaces.push_back(schema.items);
			auto tables = IcebergListTablesRequest(schema.items).Execute(context, iceberg);
			if (!tables) {
				throw InvalidInputException("Cannot list Iceberg namespace %s for conversion",
				                            QualifiedSQL(schema.items));
			}
			auto &entry = selected[QualifiedSQL(schema.items)];
			entry.parts = schema.items;
			for (auto &table : *tables) {
				if (Selected(options, schema.items, table.name)) {
					entry.tables.insert(table.name);
				}
			}
		}
	}
	for (auto &item : selected) {
		auto &selection = item.second;
		CreateSchemaInfo info;
		info.SetQualifiedName(QualifiedName(info.GetQualifiedName().Catalog(),
		                                    Identifier(StringUtil::Join(selection.parts, ".")),
		                                    info.GetQualifiedName().Name()));
		IcebergSchemaEntry schema(iceberg, info);
		schema.namespace_items = selection.parts;
		for (auto &name : selection.tables) {
			auto loaded = IcebergLoadTableRequest(selection.parts, name).Execute(context, iceberg);
			if (loaded.status_ == HTTPStatusCode::NotFound_404 && options.tables) {
				continue;
			}
			if (loaded.status_ != HTTPStatusCode::OK_200 || !loaded.result_) {
				throw InvalidInputException("Cannot load Iceberg table %s.%s for conversion (HTTP %d)",
				                            QualifiedSQL(selection.parts), IdentifierSQL(name),
				                            static_cast<int>(loaded.status_));
			}
			IcebergTable entry(iceberg, schema, name, *loaded.result_);
			entry.InitializeFromCatalogResponse(context, *loaded.result_);
			entry.LoadCredentials(context);
			auto table = make_uniq<ConversionTable>();
			table->namespace_items = selection.parts;
			table->name = entry.name;
			table->metadata = entry.table_metadata.Copy();
			try {
				ReadHistory(context, *table);
			} catch (std::exception &ex) {
				throw InvalidInputException("Cannot convert %s: %s", table->QualifiedName(), ex.what());
			}
			result.tables.push_back(std::move(table));
		}
	}
	return result;
}

static void Insert(Connection &connection, const DuckLakeMetadata &metadata, const string &table, const string &columns,
                   const vector<string> &values) {
	Query(connection, "INSERT INTO " + metadata.prefix + table + " (" + columns + ") VALUES (" +
	                      StringUtil::Join(values, ", ") + ")");
}

static string NullableNumber(optional<int64_t> value) {
	return value ? to_string(*value) : "NULL";
}

static string DefaultValue(const unique_ptr<Value> &value) {
	return value && !value->IsNull() ? StringSQL(value->ToString()) : "NULL";
}

static void WriteColumns(Connection &connection, const DuckLakeMetadata &metadata,
                         const vector<unique_ptr<IcebergColumnDefinition>> &columns, int64_t table_id,
                         int64_t snapshot_id, optional<int32_t> parent = optional<int32_t>()) {
	for (idx_t i = 0; i < columns.size(); i++) {
		auto &column = *columns[i];
		auto &write_default = column.write_default ? column.write_default : column.initial_default;
		auto default_value = DefaultValue(write_default);
		string default_kind = "'literal'";
		if (write_default && !write_default->IsNull() && write_default->ToString() == "NULL") {
			default_value = StringSQL(write_default->ToSQLString());
			default_kind = "'expression'";
		}
		Insert(
		    connection, metadata, "ducklake_column",
		    "column_id, begin_snapshot, end_snapshot, table_id, column_order, column_name, column_type, "
		    "initial_default, default_value, nulls_allowed, parent_column, default_value_type, default_value_dialect",
		    {to_string(column.id), to_string(snapshot_id), "NULL", to_string(table_id), to_string(i),
		     StringSQL(column.name), StringSQL(DuckLakeType(column.type)), DefaultValue(column.initial_default),
		     default_value, column.required ? "false" : "true", parent ? to_string(*parent) : "NULL", default_kind,
		     "'duckdb'"});
		WriteColumns(connection, metadata, column.GetChildren(), table_id, snapshot_id, column.id);
	}
}

struct ImportTableState {
	int64_t table_id;
	int32_t schema_id = -1;
	int32_t active_spec = -1;
	map<int32_t, int64_t> partitions;
	map<string, int64_t> data_files;
	map<string, int64_t> delete_files;
	int64_t next_row_id = 0;
};

static void CloseFiles(Connection &connection, const DuckLakeMetadata &metadata, const string &table,
                       const string &id_column, map<string, int64_t> &existing, const unordered_set<string> &live,
                       int64_t snapshot_id) {
	for (auto it = existing.begin(); it != existing.end();) {
		if (live.count(it->first)) {
			++it;
			continue;
		}
		Query(connection, "UPDATE " + metadata.prefix + table + " SET end_snapshot = " + to_string(snapshot_id) +
		                      " WHERE " + id_column + " = " + to_string(it->second));
		it = existing.erase(it);
	}
}

void WriteDuckLake(Connection &connection, const DuckLakeMetadata &metadata, const ConversionPlan &plan) {
	auto &tables = plan.tables;
	if (tables.empty() && plan.namespaces.empty()) {
		return;
	}
	struct Event {
		idx_t table;
		idx_t state;
	};
	vector<Event> events;
	for (idx_t i = 0; i < tables.size(); i++) {
		for (idx_t j = 0; j < tables[i]->states.size(); j++) {
			events.push_back({i, j});
		}
	}
	std::stable_sort(events.begin(), events.end(), [&](const Event &a, const Event &b) {
		auto &left = tables[a.table]->states[a.state];
		auto &right = tables[b.table]->states[b.state];
		if (left.timestamp != right.timestamp) {
			return left.timestamp < right.timestamp;
		}
		return a.table == b.table ? a.state < b.state : a.table < b.table;
	});
	auto initial = Query(connection, "SELECT next_catalog_id, next_file_id, schema_version FROM " + metadata.prefix +
	                                     "ducklake_snapshot WHERE snapshot_id = 0");
	int64_t catalog_id = initial->GetValue(0, 0).GetValue<int64_t>();
	int64_t file_id = initial->GetValue(1, 0).GetValue<int64_t>();
	int64_t schema_version = initial->GetValue(2, 0).GetValue<int64_t>();
	int64_t snapshot_id = 0;
	int64_t partition_id = 0;
	map<string, int64_t> schemas;
	auto initial_schemas =
	    Query(connection, "SELECT schema_name, schema_id FROM " + metadata.prefix + "ducklake_schema");
	for (idx_t i = 0; i < initial_schemas->RowCount(); i++) {
		schemas[QualifiedSQL({initial_schemas->GetValue(0, i).ToString()})] =
		    initial_schemas->GetValue(1, i).GetValue<int64_t>();
	}
	auto ensure_schema = [&](const vector<string> &parts, int64_t snapshot, vector<string> &changes) {
		vector<string> prefix;
		optional<int64_t> parent;
		for (auto &part : parts) {
			prefix.push_back(part);
			auto name = QualifiedSQL(prefix);
			auto existing = schemas.find(name);
			if (existing == schemas.end()) {
				auto id = catalog_id++;
				string columns =
				    "schema_id, schema_uuid, begin_snapshot, end_snapshot, schema_name, path, path_is_relative";
				vector<string> values {to_string(id),
				                       StringSQL(UUID::ToString(UUID::GenerateRandomUUID())),
				                       to_string(snapshot),
				                       "NULL",
				                       StringSQL(part),
				                       StringSQL(UUID::ToString(UUID::GenerateRandomUUID()) + "/"),
				                       "true"};
				if (metadata.v11) {
					columns += ", parent_schema_id";
					values.push_back(NullableNumber(parent));
				}
				Insert(connection, metadata, "ducklake_schema", columns, values);
				existing = schemas.emplace(name, id).first;
				schema_version++;
				changes.push_back("created_schema:" + name);
			}
			parent = existing->second;
		}
		return *parent;
	};
	vector<string> namespace_changes;
	for (auto &parts : plan.namespaces) {
		ensure_schema(parts, 1, namespace_changes);
	}
	if (events.empty() && namespace_changes.empty()) {
		return;
	}
	auto first_time = events.empty() ? Timestamp::GetCurrentTimestamp()
	                                 : tables[events.front().table]->states[events.front().state].timestamp;
	Query(connection, "UPDATE " + metadata.prefix + "ducklake_snapshot SET snapshot_time = " +
	                      ValueSQL(Value::TIMESTAMPTZ(timestamp_tz_t(first_time.value))) +
	                      " - INTERVAL 1 MICROSECOND WHERE snapshot_id = 0");
	if (events.empty()) {
		Insert(connection, metadata, "ducklake_snapshot",
		       "snapshot_id, snapshot_time, schema_version, next_catalog_id, next_file_id",
		       {"1", ValueSQL(Value::TIMESTAMPTZ(timestamp_tz_t(first_time.value))), to_string(schema_version),
		        to_string(catalog_id), to_string(file_id)});
		Insert(connection, metadata, "ducklake_snapshot_changes", "snapshot_id, changes_made, author, commit_message",
		       {"1", StringSQL(StringUtil::Join(namespace_changes, ",")), "'iceberg_to_ducklake'",
		        "'Imported empty namespaces'"});
		return;
	}
	map<idx_t, ImportTableState> imported;
	for (auto &event : events) {
		auto &table = *tables[event.table];
		auto &source = table.states[event.state];
		auto &state = imported[event.table];
		snapshot_id++;
		vector<string> changes;
		if (snapshot_id == 1) {
			changes = std::move(namespace_changes);
		}
		if (event.state == 0) {
			auto schema_id = ensure_schema(table.namespace_items, snapshot_id, changes);
			state.table_id = catalog_id++;
			Insert(connection, metadata, "ducklake_table",
			       "table_id, table_uuid, begin_snapshot, end_snapshot, schema_id, table_name, path, path_is_relative",
			       {to_string(state.table_id), StringSQL(table.metadata.table_uuid), to_string(snapshot_id), "NULL",
			        to_string(schema_id), StringSQL(table.name), StringSQL(table.metadata.table_uuid + "/"), "true"});
			changes.push_back("created_table:" + table.QualifiedName());
		}
		bool schema_changed = state.schema_id != source.schema_id;
		bool partition_changed = state.active_spec != source.spec_id;
		if (schema_changed) {
			Query(connection, "UPDATE " + metadata.prefix +
			                      "ducklake_column SET end_snapshot = " + to_string(snapshot_id) +
			                      " WHERE table_id = " + to_string(state.table_id) + " AND end_snapshot IS NULL");
			WriteColumns(connection, metadata, table.metadata.GetSchemaFromId(source.schema_id).columns, state.table_id,
			             snapshot_id);
			state.schema_id = source.schema_id;
		}
		if (schema_changed || partition_changed) {
			schema_version++;
			Insert(connection, metadata, "ducklake_schema_versions", "begin_snapshot, schema_version, table_id",
			       {to_string(snapshot_id), to_string(schema_version), to_string(state.table_id)});
			if (event.state != 0) {
				changes.push_back("altered_table:" + to_string(state.table_id));
			}
		}
		unordered_set<int32_t> needed_specs;
		needed_specs.insert(source.spec_id);
		for (auto &file : source.files) {
			needed_specs.insert(file.spec_id);
		}
		for (auto spec_id : needed_specs) {
			if (state.partitions.count(spec_id)) {
				continue;
			}
			auto &spec = table.metadata.GetPartitionSpecs().at(spec_id);
			auto id = partition_id++;
			state.partitions[spec_id] = id;
			Insert(connection, metadata, "ducklake_partition_info",
			       "partition_id, table_id, begin_snapshot, end_snapshot",
			       {to_string(id), to_string(state.table_id), to_string(snapshot_id), to_string(snapshot_id)});
			for (idx_t i = 0; i < spec.fields.size(); i++) {
				auto &field = spec.fields[i];
				Insert(connection, metadata, "ducklake_partition_column",
				       "partition_id, table_id, partition_key_index, column_id, transform",
				       {to_string(id), to_string(state.table_id), to_string(i), to_string(field.source_id),
				        StringSQL(DuckLakeTransform(field.transform))});
			}
		}
		if (partition_changed) {
			Query(connection, "UPDATE " + metadata.prefix +
			                      "ducklake_partition_info SET end_snapshot = " + to_string(snapshot_id) +
			                      " WHERE table_id = " + to_string(state.table_id) + " AND end_snapshot IS NULL");
			Insert(connection, metadata, "ducklake_partition_info",
			       "partition_id, table_id, begin_snapshot, end_snapshot",
			       {to_string(state.partitions.at(source.spec_id)), to_string(state.table_id), to_string(snapshot_id),
			        "NULL"});
			state.active_spec = source.spec_id;
		}
		unordered_set<string> live_data, live_deletes;
		for (auto &file : source.files) {
			(file.entry.data_file.content == IcebergManifestEntryContentType::DATA ? live_data : live_deletes)
			    .insert(file.Identity());
		}
		bool inserted = false;
		bool deleted = false;
		for (auto &identity : live_data) {
			inserted |= !state.data_files.count(identity);
		}
		for (auto &file : state.data_files) {
			deleted |= !live_data.count(file.first);
		}
		for (auto &identity : live_deletes) {
			deleted |= !state.delete_files.count(identity);
		}
		for (auto &file : state.delete_files) {
			inserted |= !live_deletes.count(file.first);
		}
		CloseFiles(connection, metadata, "ducklake_delete_file", "delete_file_id", state.delete_files, live_deletes,
		           snapshot_id);
		CloseFiles(connection, metadata, "ducklake_data_file", "data_file_id", state.data_files, live_data,
		           snapshot_id);
		map<string, int64_t> path_to_id;
		int64_t records = 0;
		int64_t bytes = 0;
		for (auto &file : source.files) {
			auto &data = file.entry.data_file;
			if (data.content != IcebergManifestEntryContentType::DATA) {
				continue;
			}
			records += data.record_count;
			bytes += data.file_size_in_bytes;
			auto identity = file.Identity();
			auto found = state.data_files.find(identity);
			if (found == state.data_files.end()) {
				auto id = file_id++;
				auto row_id = state.next_row_id;
				state.next_row_id += data.record_count;
				Insert(connection, metadata, "ducklake_data_file",
				       "data_file_id, table_id, begin_snapshot, end_snapshot, path, path_is_relative, file_format, "
				       "record_count, file_size_bytes, row_id_start, partition_id, footer_size",
				       {to_string(id), to_string(state.table_id), to_string(snapshot_id), "NULL",
				        StringSQL(data.file_path), "false", "'parquet'", to_string(data.record_count),
				        to_string(data.file_size_in_bytes), to_string(row_id),
				        to_string(state.partitions.at(file.spec_id)), NullableNumber(file.footer_size)});
				auto &spec = table.metadata.GetPartitionSpecs().at(file.spec_id);
				for (idx_t i = 0; i < spec.fields.size(); i++) {
					Value value;
					for (auto &partition : data.partition_info) {
						if (partition.field_id == spec.fields[i].partition_field_id) {
							value = partition.value;
						}
					}
					Insert(connection, metadata, "ducklake_file_partition_value",
					       "data_file_id, table_id, partition_key_index, partition_value",
					       {to_string(id), to_string(state.table_id), to_string(i),
					        value.IsNull() ? "NULL" : StringSQL(value.ToString())});
				}
				found = state.data_files.emplace(identity, id).first;
			}
			path_to_id[data.file_path] = found->second;
		}
		for (auto &file : source.files) {
			auto &data = file.entry.data_file;
			if (data.content == IcebergManifestEntryContentType::DATA) {
				continue;
			}
			records -= data.record_count;
			if (state.delete_files.count(file.Identity())) {
				continue;
			}
			auto id = file_id++;
			Insert(connection, metadata, "ducklake_delete_file",
			       "delete_file_id, table_id, begin_snapshot, end_snapshot, data_file_id, path, path_is_relative, "
			       "format, delete_count, file_size_bytes, footer_size",
			       {to_string(id), to_string(state.table_id), to_string(snapshot_id), "NULL",
			        to_string(path_to_id.at(ReferencedDataFile(file))), StringSQL(data.file_path), "false",
			        StringSQL(StringUtil::Lower(data.file_format)), to_string(data.record_count),
			        to_string(data.file_size_in_bytes), NullableNumber(file.footer_size)});
			state.delete_files[file.Identity()] = id;
		}
		if (records < 0) {
			throw InvalidInputException("%s: delete counts exceed live row counts", table.QualifiedName());
		}
		Query(connection,
		      "DELETE FROM " + metadata.prefix + "ducklake_table_stats WHERE table_id = " + to_string(state.table_id));
		Insert(connection, metadata, "ducklake_table_stats", "table_id, record_count, next_row_id, file_size_bytes",
		       {to_string(state.table_id), to_string(records), to_string(state.next_row_id), to_string(bytes)});
		// Missing column statistics are conservative; readers can consult Parquet footers.
		Insert(connection, metadata, "ducklake_snapshot",
		       "snapshot_id, snapshot_time, schema_version, next_catalog_id, next_file_id",
		       {to_string(snapshot_id), ValueSQL(Value::TIMESTAMPTZ(timestamp_tz_t(source.timestamp.value))),
		        to_string(schema_version), to_string(catalog_id), to_string(file_id)});
		if (inserted) {
			changes.push_back("inserted_into_table:" + to_string(state.table_id));
		}
		if (deleted) {
			changes.push_back("deleted_from_table:" + to_string(state.table_id));
		}
		Insert(connection, metadata, "ducklake_snapshot_changes",
		       "snapshot_id, changes_made, author, commit_message, commit_extra_info",
		       {to_string(snapshot_id), StringSQL(StringUtil::Join(changes, ",")), "'iceberg_to_ducklake'",
		        "'Imported retained Iceberg state'", StringSQL(source.provenance)});
	}
}

} // namespace conversion
} // namespace iceberg

TableFunctionSet IcebergFunctions::GetIcebergToDuckLakeFunction() {
	return iceberg::conversion::ConversionFunction(true);
}

} // namespace duckdb
