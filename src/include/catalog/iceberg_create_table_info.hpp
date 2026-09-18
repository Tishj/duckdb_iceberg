#pragma once

#include "duckdb/common/case_insensitive_map.hpp"
#include "duckdb/common/shared_ptr.hpp"
#include "core/metadata/partition/iceberg_partition_spec.hpp"

namespace duckdb {
class IcebergTableSchema;

//! Backend-independent inputs for creating an Iceberg table.
struct IcebergCreateTableInfo {
	string name;
	shared_ptr<IcebergTableSchema> schema;
	IcebergPartitionSpec partition_spec;
	idx_t iceberg_version;
	case_insensitive_map_t<string> table_properties;
	string location;
};

} // namespace duckdb
