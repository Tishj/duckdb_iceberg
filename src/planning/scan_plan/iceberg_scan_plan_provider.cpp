#include "catalog/iceberg_catalog_backend.hpp"
#include "planning/scan_plan/iceberg_scan_plan_provider.hpp"

#include "catalog/rest/api/iceberg_expression.hpp"
#include "catalog/rest/api/iceberg_type.hpp"
#include "catalog/rest/catalog_entry/table/iceberg_table_schema_version.hpp"
#include "catalog/rest/iceberg_catalog.hpp"
#include "duckdb/common/numeric_utils.hpp"
#include "duckdb/storage/table/row_group_reorderer.hpp"
#include "planning/pruning/iceberg_table_filter.hpp"
#include "planning/scan_order/iceberg_scan_order.hpp"

namespace duckdb {

namespace {

enum class ScanPlanningMode : uint8_t { UNSPECIFIED, SERVER_SIDE_ONLY, CLIENT_SIDE_ONLY };

static ScanPlanningMode GetScanPlanningMode(optional_ptr<IcebergTableSchemaVersion> table) {
	if (!table) {
		return ScanPlanningMode::CLIENT_SIDE_ONLY;
	}

	auto &config = table->table_info.config;
	auto it = config.find("scan-planning-mode");
	if (it == config.end()) {
		return ScanPlanningMode::UNSPECIFIED;
	}
	auto &mode = it->second;
	if (StringUtil::CIEquals(mode, "client")) {
		return ScanPlanningMode::CLIENT_SIDE_ONLY;
	}
	if (StringUtil::CIEquals(mode, "server")) {
		return ScanPlanningMode::SERVER_SIDE_ONLY;
	}
	throw InvalidConfigurationException("Table's config 'scan-planning-mode' has unrecognized option: %s", mode);
}

} // namespace

unique_ptr<IcebergScanPlanProvider> IcebergScanPlanProvider::Create(IcebergScanPlanState &shared_state,
                                                                    IcebergScanPlanContext context,
                                                                    optional_ptr<IcebergTableSchemaVersion> table_entry,
                                                                    const IcebergTableFilters &table_filters,
                                                                    const IcebergScanOrder &scan_order,
                                                                    bool server_side_planning_enabled) {
	if (!context.snapshot.snapshot) {
		return make_uniq<ClientSideScanPlanProvider>(shared_state, context);
	}

	auto scan_planning_mode = GetScanPlanningMode(table_entry);
	if (scan_planning_mode == ScanPlanningMode::UNSPECIFIED) {
		Value value;
		if (context.context.TryGetCurrentSetting("iceberg_use_server_side_scan_planning", value) && !value.IsNull() &&
		    value.type().id() == LogicalTypeId::BOOLEAN && !value.GetValue<bool>()) {
			scan_planning_mode = ScanPlanningMode::CLIENT_SIDE_ONLY;
		}
	}
	if (!table_entry || context.transaction_data || table_entry->table_info.IsRenamed() ||
	    scan_planning_mode == ScanPlanningMode::CLIENT_SIDE_ONLY) {
		server_side_planning_enabled = false;
	}

	unique_ptr<IcebergScanPlanProvider> provider;
	if (server_side_planning_enabled) {
		auto &table = table_entry->table_info;
		provider = table.catalog.GetBackend().PlanScan(context, table, table_filters, scan_order);
	}
	if (!provider && scan_planning_mode == ScanPlanningMode::SERVER_SIDE_ONLY) {
		D_ASSERT(table_entry);
		throw BinderException(
		    "Unable to plan scan for table %s, but table's config disabled non-server-side scan planning",
		    table_entry->table_info.name);
	}
	if (!provider) {
		provider = make_uniq<ClientSideScanPlanProvider>(shared_state, context);
	}
	return provider;
}

} // namespace duckdb
