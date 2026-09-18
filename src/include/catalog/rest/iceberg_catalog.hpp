#pragma once

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/common/enums/access_mode.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/main/secret/secret_manager.hpp"
#include "duckdb/parser/parsed_data/attach_info.hpp"
#include "duckdb/storage/storage_extension.hpp"

#include "catalog/rest/iceberg_schema_set.hpp"
#include "common/iceberg_utils.hpp"

namespace duckdb {

class IcebergCatalogBackend;
class IcebergSchemaEntry;
struct IcebergTable;

class IcebergCatalog : public Catalog {
public:
	explicit IcebergCatalog(AttachedDatabase &db_p, AccessMode access_mode, unique_ptr<IcebergCatalogBackend> backend,
	                        IcebergAttachOptions &attach_options, const Identifier &default_schema);
	~IcebergCatalog() override;

public:
	IcebergCatalogBackend &GetBackend() const {
		return *backend;
	}
	//! Whether or not this catalog should search a specific type with the standard priority
	CatalogLookupBehavior CatalogTypeLookupRule(CatalogType type) const override {
		switch (type) {
		case CatalogType::TABLE_FUNCTION_ENTRY:
		case CatalogType::SCALAR_FUNCTION_ENTRY:
		case CatalogType::AGGREGATE_FUNCTION_ENTRY:
			return CatalogLookupBehavior::NEVER_LOOKUP;
		default:
			return CatalogLookupBehavior::STANDARD;
		}
	}
	bool CheckAmbiguousCatalogOrSchema(ClientContext &context, const Identifier &schema) override {
		return false;
	}
	optional<Identifier> GetDefaultSchema() const override;
	ErrorData SupportsCreateTable(BoundCreateTableInfo &info) override;

public:
	void Initialize(bool load_builtin) override;
	string GetCatalogType() override {
		return "iceberg";
	}
	bool SupportsTimeTravel() const override {
		return true;
	}
	void DropSchema(ClientContext &context, DropInfo &info) override;
	optional_ptr<CatalogEntry> CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) override;
	void ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) override;
	IcebergSchemaSet &GetSchemas();
	optional_ptr<SchemaCatalogEntry> LookupSchema(CatalogTransaction transaction, const EntryLookupInfo &schema_lookup,
	                                              OnEntryNotFound if_not_found) override;
	PhysicalOperator &PlanInsert(ClientContext &context, PhysicalPlanGenerator &planner, LogicalInsert &op,
	                             optional_ptr<PhysicalOperator> plan) override;
	PhysicalOperator &PlanCreateTableAs(ClientContext &context, PhysicalPlanGenerator &planner, LogicalCreateTable &op,
	                                    PhysicalOperator &plan) override;
	PhysicalOperator &PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op,
	                             PhysicalOperator &plan) override;
	//! Shared delete-planning body both PlanDelete and MERGE build on; only PlanDelete additionally opts the
	//! standalone DELETE into metadata-only deletes, so MERGE must plan its delete action through here directly.
	PhysicalOperator &PlanDeleteOperation(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op,
	                                      PhysicalOperator &plan);
	PhysicalOperator &PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner, LogicalUpdate &op,
	                             PhysicalOperator &plan) override;
	PhysicalOperator &PlanMergeInto(ClientContext &context, PhysicalPlanGenerator &planner, LogicalMergeInto &op,
	                                PhysicalOperator &plan) override;
	unique_ptr<LogicalOperator> BindCreateIndex(Binder &binder, CreateStatement &stmt, TableCatalogEntry &table,
	                                            unique_ptr<LogicalOperator> plan) override;
	DatabaseSize GetDatabaseSize(ClientContext &context) override;
	//! Whether or not this is an in-memory Iceberg database
	bool InMemory() override;
	string GetDBPath() override;
	//! Allow ATTACH OR REPLACE to actually re-attach when iceberg-specific options change
	bool HasConflictingAttachOptions(const string &path, const AttachOptions &options) override;
	void SetAttachOptions(const unordered_map<string, Value> &options);
	static string GetOnlyMergeOnReadSupportedErrorMessage(const string &table_name, const string &property,
	                                                      const string &property_value);

public:
	AccessMode access_mode;
	//! attach options
	IcebergAttachOptions attach_options;
	Identifier default_schema;

private:
	unique_ptr<IcebergCatalogBackend> backend;
	//! Normalized attach options (after core stripping) used to detect a conflicting ATTACH OR REPLACE
	unordered_map<string, Value> normalized_attach_options;

public:
	IcebergSchemaSet schemas;
};

} // namespace duckdb
