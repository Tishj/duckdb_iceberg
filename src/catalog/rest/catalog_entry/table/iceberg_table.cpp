#include "catalog/rest/catalog_entry/table/iceberg_table.hpp"

#include "duckdb/common/case_insensitive_map.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/parser/expression/columnref_expression.hpp"
#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/common/exception/transaction_exception.hpp"
#include "duckdb/common/types/string.hpp"
#include "duckdb/common/types/timestamp.hpp"
#include "duckdb/common/operator/cast_operators.hpp"
#include "duckdb/parser/column_definition.hpp"
#include "duckdb/logging/logger.hpp"
#include "duckdb/common/types/uuid.hpp"

#include "catalog/iceberg_catalog_backend.hpp"
#include "catalog/rest/transaction/iceberg_transaction.hpp"
#include "catalog/rest/transaction/iceberg_transaction_data.hpp"
#include "catalog/rest/catalog_entry/schema/iceberg_schema_entry.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "catalog/rest/storage/iceberg_table_secret_provider.hpp"
#include "core/expression/iceberg_transform.hpp"
#include "common/iceberg_utils.hpp"

#include <climits>

namespace duckdb {

bool IcebergTable::IsRenamed() const {
	return original_name != name;
}

const string &IcebergTable::BaseFilePath() const {
	return table_metadata.location;
}

IcebergTableCredentials IcebergTable::GetVendedCredentials(ClientContext &context) const {
	return catalog.GetBackend().GetTableCredentials(context, *this);
}

optional_ptr<CatalogEntry> IcebergTable::CreateSchemaVersion(const IcebergTableSchema &table_schema) {
	CreateTableInfo info;
	info.SetTableName(Identifier(name));
	for (auto &col : table_schema.columns) {
		info.columns.AddColumn(col->GetColumnDefinition());
	}

	auto table_entry = make_uniq<IcebergTableSchemaVersion>(*this, catalog, schema, info, table_schema.schema_id);
	if (!table_entry->internal) {
		table_entry->internal = schema.internal;
	}
	auto result = table_entry.get();
	if (result->name.empty()) {
		throw InternalException("IcebergTableSet::CreateEntry called with empty name");
	}

	schema_versions.emplace(table_schema.schema_id, std::move(table_entry));
	return result;
}

idx_t IcebergTable::GetMaxSchemaId() {
	idx_t max_schema_id = 0;
	if (schema_versions.empty()) {
		throw CatalogException("No schema versions found for table '%s.%s'", schema.name.GetIdentifierName(), name);
	}
	for (auto &schema : schema_versions) {
		if (schema.first > max_schema_id) {
			max_schema_id = schema.first;
		}
	}
	return max_schema_id;
}

idx_t IcebergTable::GetNextPartitionSpecId() {
	idx_t max_partition_spec_id = table_metadata.default_spec_id;
	for (auto &partition_spec : table_metadata.GetPartitionSpecs()) {
		auto &partition_spec_id = partition_spec.first;
		if (partition_spec_id > max_partition_spec_id) {
			max_partition_spec_id = partition_spec_id;
		}
	}
	return max_partition_spec_id + 1;
}

idx_t IcebergTable::GetNextSortOrderId() {
	idx_t max_sort_order_id = 0;
	if (table_metadata.default_sort_order_id.IsValid()) {
		max_sort_order_id = table_metadata.default_sort_order_id.GetIndex();
	}
	for (auto &sort_order : table_metadata.GetSortOrderSpecs()) {
		auto &sort_order_id = sort_order.first;
		if (sort_order_id > max_sort_order_id) {
			max_sort_order_id = sort_order_id;
		}
	}
	return max_sort_order_id + 1;
}

optional<int64_t> IcebergTable::GetExistingSpecId(IcebergPartitionSpec &spec) {
	for (auto &existing_spec : table_metadata.GetPartitionSpecs()) {
		if (spec.Equals(existing_spec.second)) {
			return existing_spec.first;
		}
	}
	return std::nullopt;
}

optional<int64_t> IcebergTable::GetExistingSortOrderId(IcebergSortOrder &spec) {
	for (auto &existing_sort_order : table_metadata.GetSortOrderSpecs()) {
		if (spec.Equals(existing_sort_order.second)) {
			return existing_sort_order.first;
		}
	}
	return std::nullopt;
}

IcebergPartitionSpec IcebergTable::BuildPartitionSpec(const vector<unique_ptr<ParsedExpression>> &partition_keys,
                                                      const IcebergTableSchema &schema, int32_t spec_id,
                                                      idx_t base_partition_field_id) {
	IcebergPartitionSpec new_spec(spec_id);

	for (auto &key : partition_keys) {
		vector<reference<const IcebergColumnDefinition>> source_columns;
		auto transform = IcebergTransform::FromExpression(*key, schema, source_columns);
		if (source_columns.size() != 1) {
			throw InvalidInputException("Multi-argument transforms are not supported yet!");
		}
		auto source_id = source_columns[0].get().id;
		auto column_name = source_columns[0].get().name;

		IcebergPartitionSpecField field;
		field.transform = transform;
		field.source_id = source_id;
		field.partition_field_id = base_partition_field_id + new_spec.fields.size();
		// transform field names cannot be the column name. Otherwise Lakekeeper complains
		field.SetPartitionSpecFieldName(column_name);
		new_spec.fields.push_back(std::move(field));
	}

	return new_spec;
}

IcebergSortOrder IcebergTable::BuildSortOrder(const vector<OrderByNode> &orders, const IcebergTableSchema &schema,
                                              int32_t sort_order_id) {
	IcebergSortOrder new_sort_order(sort_order_id);

	for (auto &order : orders) {
		vector<reference<const IcebergColumnDefinition>> source_columns;
		auto transform = IcebergTransform::FromExpression(*order.expression, schema, source_columns);
		if (source_columns.size() != 1) {
			throw InvalidInputException("Multi-argument transforms are not supported yet!");
		}
		auto source_id = source_columns[0].get().id;

		IcebergSortOrderField field;
		field.source_id = source_id;
		field.transform = transform;
		field.direction = order.type == OrderType::ASCENDING ? "asc" : "desc";
		field.null_order = order.null_order == OrderByNullType::NULLS_FIRST ? "nulls-first" : "nulls-last";
		new_sort_order.fields.push_back(std::move(field));
	}
	return new_sort_order;
}

void IcebergTable::SetPartitionedBy(IcebergTransaction &transaction,
                                    const vector<unique_ptr<ParsedExpression>> &partition_keys,
                                    const IcebergTableSchema &schema) {
	idx_t base_partition_field_id = 1000;
	if (table_metadata.HasLastPartitionId()) {
		base_partition_field_id = table_metadata.GetLastPartitionFieldId() + 1;
	}
	auto new_spec_id = static_cast<int32_t>(GetNextPartitionSpecId());

	auto new_spec = BuildPartitionSpec(partition_keys, schema, new_spec_id, base_partition_field_id);

	// if spec definition already exists in a previous spec definition, set it to that spec id
	// (some catalog may allow duplicate definitions, others not)
	auto existing_spec_id = GetExistingSpecId(new_spec);
	auto &transaction_data = GetOrCreateTransactionData(transaction);
	if (existing_spec_id) {
		table_metadata.default_spec_id = *existing_spec_id;
		transaction_data.TableSetDefaultSpec();
		return;
	}

	table_metadata.partition_specs.emplace(new_spec_id, std::move(new_spec));
	table_metadata.default_spec_id = new_spec_id;
	transaction_data.TableAddPartitionSpec();
	transaction_data.TableSetDefaultSpec();
}

void IcebergTable::SetSortedBy(IcebergTransaction &transaction, const vector<OrderByNode> &orders,
                               const IcebergTableSchema &schema, bool first_sort_spec) {
	idx_t new_sort_order_id = 0;
	if (!first_sort_spec) {
		new_sort_order_id = GetNextSortOrderId();
	}

	auto new_sort_order = BuildSortOrder(orders, schema, static_cast<int32_t>(new_sort_order_id));

	// if spec definition already exists in a previous spec definition, set it to that spec id
	// (some catalog may allow duplicate definitions, others not)
	auto existing_sort_order_id = GetExistingSortOrderId(new_sort_order);
	if (existing_sort_order_id) {
		table_metadata.default_sort_order_id = *existing_sort_order_id;
		if (!first_sort_spec) {
			auto &transaction_data = GetOrCreateTransactionData(transaction);
			transaction_data.TableSetDefaultSortOrder();
		}
		return;
	}

	table_metadata.sort_specs.emplace(new_sort_order_id, std::move(new_sort_order));
	table_metadata.default_sort_order_id = new_sort_order_id;
	if (!first_sort_spec) {
		auto &transaction_data = GetOrCreateTransactionData(transaction);
		transaction_data.TableAddSortOrder();
		transaction_data.TableSetDefaultSortOrder();
	}
}

optional_ptr<CatalogEntry> IcebergTable::GetSchemaVersion(optional_ptr<BoundAtClause> at) {
	if (table_metadata.snapshots.empty()) {
		return schema_versions[table_metadata.GetCurrentSchemaId()].get();
	}

	D_ASSERT(!schema_versions.empty());
	auto snapshot_lookup = IcebergSnapshotLookup::FromAtClause(at);
	auto snapshot_info = table_metadata.GetSnapshot(snapshot_lookup);

	int32_t schema_id;
	if (!snapshot_lookup.IsLatest() && snapshot_info.snapshot) {
		schema_id = snapshot_info.snapshot->GetSchemaId();
	} else {
		schema_id = table_metadata.GetCurrentSchemaId();
	}
	return schema_versions[schema_id].get();
}

idx_t IcebergTable::GetIcebergVersion() const {
	return table_metadata.iceberg_version;
}

void IcebergTable::LoadCredentials(ClientContext &context) const {
	catalog.GetBackend().LoadCredentials(context, *this);
}

optional_ptr<CatalogEntry> IcebergTable::GetLatestSchema() {
	return GetSchemaVersion(nullptr);
}

string IcebergTable::GetTableKey(const IcebergCatalog &catalog, const vector<string> &namespace_items,
                                 const string &table_name) {
	return catalog.GetBackend().GetTableKey(namespace_items, table_name);
}

string IcebergTable::GetTableKey() const {
	return GetTableKey(catalog, schema.namespace_items, name);
}

bool IcebergTable::HasTransactionUpdates() const {
	if (!transaction_data) {
		return false;
	}
	auto &data = *transaction_data;
	if (!data.updates.empty()) {
		return true;
	}
	if (!data.requirements.empty()) {
		return true;
	}
	if (data.pending_current_schema_id.has_value()) {
		return true;
	}
	if (data.assert_schema_id) {
		return true;
	}
	return false;
}

void IcebergTable::RefreshFromCatalog(ClientContext &context) {
	if (!catalog.GetBackend().LoadTable(context, *this, true)) {
		throw CatalogException("Table %s no longer exists", name);
	}
}

IcebergTable IcebergTable::Copy() const {
	auto clone = IcebergTable(catalog, schema, name, table_metadata.Copy());
	clone.config = config;
	clone.backend_data = backend_data;
	return clone;
}

IcebergTableMetadata IcebergTable::CreateMetadataFromLog(ClientContext &context,
                                                         timestamp_ms_t transaction_start_ms) const {
	auto &log = table_metadata.metadata_log;

	optional_idx log_item_index;
	for (idx_t i = log.size(); i-- > 0;) {
		if (log[i].timestamp_ms <= transaction_start_ms) {
			log_item_index = i;
			break;
		}
	}
	if (!log_item_index.IsValid()) {
		auto timestamp = duckdb::Cast::Operation<timestamp_ms_t, timestamp_t>(transaction_start_ms);
		throw InvalidConfigurationException(
		    "Cannot reconstruct table '%s' at the transaction start (%s) because its metadata-log has no entry from "
		    "that time or earlier. Set iceberg_use_metadata_log = false to accept the latest table state resolved by "
		    "this transaction instead",
		    GetTableKey(), Timestamp::ToString(timestamp));
	}

	auto fs = make_shared_ptr<CachingFileSystemWrapper>(FileSystem::GetFileSystem(context), *context.db);
	auto &path = log[log_item_index.GetIndex()].metadata_file;
	auto parsed_metadata = IcebergTableMetadata::Parse(path, *fs, "");

	return IcebergTableMetadata::FromTableMetadata(parsed_metadata);
}

IcebergTable IcebergTable::Copy(IcebergTransaction &iceberg_transaction) const {
	auto locked_context = iceberg_transaction.context.lock();
	auto &context = *locked_context;

	auto ret = Copy();
	auto transaction_start_ms = IcebergUtils::GetTransactionStartTimeMS(context);

	if (table_metadata.last_updated_ms <= transaction_start_ms) {
		return ret;
	}
	bool use_metadata_log = true;
	Value val;
	if (context.TryGetCurrentSetting("iceberg_use_metadata_log", val)) {
		if (!val.IsNull() && val.type().id() == LogicalTypeId::BOOLEAN) {
			use_metadata_log = val.GetValue<bool>();
		}
	}

	if (!use_metadata_log) {
		return ret;
	}
	if (table_metadata.metadata_log.empty()) {
		throw InvalidConfigurationException(
		    "Cannot reconstruct table '%s' at the transaction start because iceberg_use_metadata_log is enabled, "
		    "but the table metadata does not contain a metadata-log. Set iceberg_use_metadata_log = false to "
		    "accept the latest table state resolved by this transaction instead",
		    GetTableKey());
	}

	LoadCredentials(context);
	ret.table_metadata = ret.CreateMetadataFromLog(context, transaction_start_ms);
	return ret;
}

void IcebergTable::InitSchemaVersions() {
	schema_versions.clear();
	auto &schemas = table_metadata.GetSchemas();
	schemas.ForEachSchema([&](const IcebergTableSchema &schema) { CreateSchemaVersion(schema); });
}

IcebergTable::IcebergTable(IcebergCatalog &catalog, IcebergSchemaEntry &schema, const string &name,
                           IcebergTableMetadata metadata)
    : catalog(catalog), schema(schema), name(name), table_metadata(std::move(metadata)), original_name(name) {
}

shared_ptr<IcebergTable> IcebergTable::CreatePlaceholder(IcebergCatalog &catalog, IcebergSchemaEntry &schema,
                                                         const string &name) {
	return make_shared_ptr<IcebergTable>(catalog, schema, name, IcebergTableMetadata(IcebergTableMetadataSchemas {}));
}

IcebergTransactionData &IcebergTable::GetOrCreateTransactionData(IcebergTransaction &transaction) {
	lock_guard<mutex> guard(transaction.lock);
	if (!transaction_data) {
		auto context = transaction.context.lock();
		transaction_data = make_uniq<IcebergTransactionData>(*context, transaction, *this);
	}
	return *transaction_data;
}

} // namespace duckdb
