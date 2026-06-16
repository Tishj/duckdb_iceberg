
#include "rest_catalog/objects/table_metadata.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TableMetadata::TableMetadata(int32_t format_version_p, string table_uuid_p, optional<string> location_p,
                             optional<int64_t> last_updated_ms_p, optional<int64_t> next_row_id_p,
                             optional<case_insensitive_map_t<string>> properties_p, optional<vector<Schema>> schemas_p,
                             optional<int32_t> current_schema_id_p, optional<int32_t> last_column_id_p,
                             optional<vector<PartitionSpec>> partition_specs_p, optional<int32_t> default_spec_id_p,
                             optional<int32_t> last_partition_id_p, optional<vector<SortOrder>> sort_orders_p,
                             optional<int32_t> default_sort_order_id_p,
                             optional<vector<EncryptedKey>> encryption_keys_p, optional<vector<Snapshot>> snapshots_p,
                             optional<SnapshotReferences> refs_p, optional<int64_t> current_snapshot_id_p,
                             optional<int64_t> last_sequence_number_p, optional<SnapshotLog> snapshot_log_p,
                             optional<MetadataLog> metadata_log_p, optional<vector<StatisticsFile>> statistics_p,
                             optional<vector<PartitionStatisticsFile>> partition_statistics_p)
    : format_version(std::move(format_version_p)), table_uuid(std::move(table_uuid_p)), location(std::move(location_p)),
      last_updated_ms(std::move(last_updated_ms_p)), next_row_id(std::move(next_row_id_p)),
      properties(std::move(properties_p)), schemas(std::move(schemas_p)),
      current_schema_id(std::move(current_schema_id_p)), last_column_id(std::move(last_column_id_p)),
      partition_specs(std::move(partition_specs_p)), default_spec_id(std::move(default_spec_id_p)),
      last_partition_id(std::move(last_partition_id_p)), sort_orders(std::move(sort_orders_p)),
      default_sort_order_id(std::move(default_sort_order_id_p)), encryption_keys(std::move(encryption_keys_p)),
      snapshots(std::move(snapshots_p)), refs(std::move(refs_p)), current_snapshot_id(std::move(current_snapshot_id_p)),
      last_sequence_number(std::move(last_sequence_number_p)), snapshot_log(std::move(snapshot_log_p)),
      metadata_log(std::move(metadata_log_p)), statistics(std::move(statistics_p)),
      partition_statistics(std::move(partition_statistics_p)) {
}
TableMetadata::TableMetadata(const TableMetadata &other)
    : format_version(other.format_version), table_uuid(other.table_uuid),
      location((other.location.has_value() ? optional<string>((*other.location)) : optional<string>())),
      last_updated_ms(
          (other.last_updated_ms.has_value() ? optional<int64_t>((*other.last_updated_ms)) : optional<int64_t>())),
      next_row_id((other.next_row_id.has_value() ? optional<int64_t>((*other.next_row_id)) : optional<int64_t>())),
      properties((other.properties.has_value() ? optional<case_insensitive_map_t<string>>(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : (*other.properties)) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }()))
                                               : optional<case_insensitive_map_t<string>>())),
      schemas((other.schemas.has_value() ? optional<vector<Schema>>(([&]() {
	      vector<Schema> copied;
	      copied.reserve((*other.schemas).size());
	      for (const auto &item : (*other.schemas)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                         : optional<vector<Schema>>())),
      current_schema_id(
          (other.current_schema_id.has_value() ? optional<int32_t>((*other.current_schema_id)) : optional<int32_t>())),
      last_column_id(
          (other.last_column_id.has_value() ? optional<int32_t>((*other.last_column_id)) : optional<int32_t>())),
      partition_specs((other.partition_specs.has_value() ? optional<vector<PartitionSpec>>(([&]() {
	      vector<PartitionSpec> copied;
	      copied.reserve((*other.partition_specs).size());
	      for (const auto &item : (*other.partition_specs)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                         : optional<vector<PartitionSpec>>())),
      default_spec_id(
          (other.default_spec_id.has_value() ? optional<int32_t>((*other.default_spec_id)) : optional<int32_t>())),
      last_partition_id(
          (other.last_partition_id.has_value() ? optional<int32_t>((*other.last_partition_id)) : optional<int32_t>())),
      sort_orders((other.sort_orders.has_value() ? optional<vector<SortOrder>>(([&]() {
	      vector<SortOrder> copied;
	      copied.reserve((*other.sort_orders).size());
	      for (const auto &item : (*other.sort_orders)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                 : optional<vector<SortOrder>>())),
      default_sort_order_id((other.default_sort_order_id.has_value() ? optional<int32_t>((*other.default_sort_order_id))
                                                                     : optional<int32_t>())),
      encryption_keys((other.encryption_keys.has_value() ? optional<vector<EncryptedKey>>(([&]() {
	      vector<EncryptedKey> copied;
	      copied.reserve((*other.encryption_keys).size());
	      for (const auto &item : (*other.encryption_keys)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                         : optional<vector<EncryptedKey>>())),
      snapshots((other.snapshots.has_value() ? optional<vector<Snapshot>>(([&]() {
	      vector<Snapshot> copied;
	      copied.reserve((*other.snapshots).size());
	      for (const auto &item : (*other.snapshots)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                             : optional<vector<Snapshot>>())),
      refs((other.refs.has_value() ? optional<SnapshotReferences>((*other.refs).Copy())
                                   : optional<SnapshotReferences>())),
      current_snapshot_id((other.current_snapshot_id.has_value() ? optional<int64_t>((*other.current_snapshot_id))
                                                                 : optional<int64_t>())),
      last_sequence_number((other.last_sequence_number.has_value() ? optional<int64_t>((*other.last_sequence_number))
                                                                   : optional<int64_t>())),
      snapshot_log((other.snapshot_log.has_value() ? optional<SnapshotLog>((*other.snapshot_log).Copy())
                                                   : optional<SnapshotLog>())),
      metadata_log((other.metadata_log.has_value() ? optional<MetadataLog>((*other.metadata_log).Copy())
                                                   : optional<MetadataLog>())),
      statistics((other.statistics.has_value() ? optional<vector<StatisticsFile>>(([&]() {
	      vector<StatisticsFile> copied;
	      copied.reserve((*other.statistics).size());
	      for (const auto &item : (*other.statistics)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                               : optional<vector<StatisticsFile>>())),
      partition_statistics((other.partition_statistics.has_value() ? optional<vector<PartitionStatisticsFile>>(([&]() {
	      vector<PartitionStatisticsFile> copied;
	      copied.reserve((*other.partition_statistics).size());
	      for (const auto &item : (*other.partition_statistics)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                                   : optional<vector<PartitionStatisticsFile>>())) {
}
TableMetadata::TableMetadata(TableMetadata &&other) : TableMetadata(static_cast<const TableMetadata &>(other)) {
}

TableMetadataBuilder::TableMetadataBuilder() {
}

TableMetadataBuilder &TableMetadataBuilder::SetFormatVersion(int32_t value) {
	format_version_.emplace(std::move(value));
	has_format_version_ = true;
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetTableUuid(string value) {
	table_uuid_.emplace(std::move(value));
	has_table_uuid_ = true;
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetLocation(string value) {
	location_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetLastUpdatedMs(int64_t value) {
	last_updated_ms_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetNextRowId(int64_t value) {
	next_row_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetProperties(case_insensitive_map_t<string> value) {
	properties_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetSchemas(vector<Schema> value) {
	schemas_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetCurrentSchemaId(int32_t value) {
	current_schema_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetLastColumnId(int32_t value) {
	last_column_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetPartitionSpecs(vector<PartitionSpec> value) {
	partition_specs_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetDefaultSpecId(int32_t value) {
	default_spec_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetLastPartitionId(int32_t value) {
	last_partition_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetSortOrders(vector<SortOrder> value) {
	sort_orders_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetDefaultSortOrderId(int32_t value) {
	default_sort_order_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetEncryptionKeys(vector<EncryptedKey> value) {
	encryption_keys_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetSnapshots(vector<Snapshot> value) {
	snapshots_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetRefs(SnapshotReferences value) {
	refs_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetCurrentSnapshotId(int64_t value) {
	current_snapshot_id_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetLastSequenceNumber(int64_t value) {
	last_sequence_number_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetSnapshotLog(SnapshotLog value) {
	snapshot_log_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetMetadataLog(MetadataLog value) {
	metadata_log_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetStatistics(vector<StatisticsFile> value) {
	statistics_.emplace(std::move(value));
	return *this;
}

TableMetadataBuilder &TableMetadataBuilder::SetPartitionStatistics(vector<PartitionStatisticsFile> value) {
	partition_statistics_.emplace(std::move(value));
	return *this;
}

TableMetadata TableMetadataBuilder::Build() {
	if (!has_format_version_) {
		throw InvalidInputException("TableMetadata required property 'format-version' is missing");
	}
	if (!has_table_uuid_) {
		throw InvalidInputException("TableMetadata required property 'table-uuid' is missing");
	}
	auto result = TableMetadata(std::move(*format_version_), std::move(*table_uuid_), std::move(location_),
	                            std::move(last_updated_ms_), std::move(next_row_id_), std::move(properties_),
	                            std::move(schemas_), std::move(current_schema_id_), std::move(last_column_id_),
	                            std::move(partition_specs_), std::move(default_spec_id_), std::move(last_partition_id_),
	                            std::move(sort_orders_), std::move(default_sort_order_id_), std::move(encryption_keys_),
	                            std::move(snapshots_), std::move(refs_), std::move(current_snapshot_id_),
	                            std::move(last_sequence_number_), std::move(snapshot_log_), std::move(metadata_log_),
	                            std::move(statistics_), std::move(partition_statistics_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TableMetadataBuilder::TryBuild(optional<TableMetadata> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string TableMetadata::TryFromJSON(yyjson_val *obj, TableMetadataBuilder &builder) {
	try {
		auto format_version_val = yyjson_obj_get(obj, "format-version");
		if (!format_version_val) {
			throw InvalidInputException("TableMetadata required property 'format-version' is missing");
		} else {
			int32_t format_version;
			if (yyjson_is_int(format_version_val)) {
				format_version = yyjson_get_int(format_version_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'format_version' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(format_version_val)));
			}
			builder.SetFormatVersion(std::move(format_version));
		}
		auto table_uuid_val = yyjson_obj_get(obj, "table-uuid");
		if (!table_uuid_val) {
			throw InvalidInputException("TableMetadata required property 'table-uuid' is missing");
		} else {
			string table_uuid;
			if (yyjson_is_str(table_uuid_val)) {
				table_uuid = yyjson_get_str(table_uuid_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'table_uuid' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(table_uuid_val)));
			}
			builder.SetTableUuid(std::move(table_uuid));
		}
		auto location_val = yyjson_obj_get(obj, "location");
		if (location_val) {
			string location;
			if (yyjson_is_str(location_val)) {
				location = yyjson_get_str(location_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TableMetadata property 'location' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(location_val)));
			}
			builder.SetLocation(std::move(location));
		}
		auto last_updated_ms_val = yyjson_obj_get(obj, "last-updated-ms");
		if (last_updated_ms_val) {
			int64_t last_updated_ms;
			if (yyjson_is_sint(last_updated_ms_val)) {
				last_updated_ms = yyjson_get_sint(last_updated_ms_val);
			} else if (yyjson_is_uint(last_updated_ms_val)) {
				last_updated_ms = yyjson_get_uint(last_updated_ms_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'last_updated_ms' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(last_updated_ms_val)));
			}
			builder.SetLastUpdatedMs(std::move(last_updated_ms));
		}
		auto next_row_id_val = yyjson_obj_get(obj, "next-row-id");
		if (next_row_id_val) {
			int64_t next_row_id;
			if (yyjson_is_sint(next_row_id_val)) {
				next_row_id = yyjson_get_sint(next_row_id_val);
			} else if (yyjson_is_uint(next_row_id_val)) {
				next_row_id = yyjson_get_uint(next_row_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'next_row_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(next_row_id_val)));
			}
			builder.SetNextRowId(std::move(next_row_id));
		}
		auto properties_val = yyjson_obj_get(obj, "properties");
		if (properties_val) {
			case_insensitive_map_t<string> properties;
			if (yyjson_is_obj(properties_val)) {
				size_t idx, max;
				yyjson_val *key, *val;
				yyjson_obj_foreach(properties_val, idx, max, key, val) {
					auto key_str = yyjson_get_str(key);
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "TableMetadata property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					properties.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("TableMetadata property 'properties' is not of type 'object'");
			}
			builder.SetProperties(std::move(properties));
		}
		auto schemas_val = yyjson_obj_get(obj, "schemas");
		if (schemas_val) {
			vector<Schema> schemas;
			if (yyjson_is_arr(schemas_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(schemas_val, idx, max, val) {
					auto tmp = Schema::FromJSON(val);
					schemas.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TableMetadata property 'schemas' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(schemas_val)));
			}
			builder.SetSchemas(std::move(schemas));
		}
		auto current_schema_id_val = yyjson_obj_get(obj, "current-schema-id");
		if (current_schema_id_val) {
			int32_t current_schema_id;
			if (yyjson_is_int(current_schema_id_val)) {
				current_schema_id = yyjson_get_int(current_schema_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'current_schema_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(current_schema_id_val)));
			}
			builder.SetCurrentSchemaId(std::move(current_schema_id));
		}
		auto last_column_id_val = yyjson_obj_get(obj, "last-column-id");
		if (last_column_id_val) {
			int32_t last_column_id;
			if (yyjson_is_int(last_column_id_val)) {
				last_column_id = yyjson_get_int(last_column_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'last_column_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(last_column_id_val)));
			}
			builder.SetLastColumnId(std::move(last_column_id));
		}
		auto partition_specs_val = yyjson_obj_get(obj, "partition-specs");
		if (partition_specs_val) {
			vector<PartitionSpec> partition_specs;
			if (yyjson_is_arr(partition_specs_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(partition_specs_val, idx, max, val) {
					auto tmp = PartitionSpec::FromJSON(val);
					partition_specs.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'partition_specs' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(partition_specs_val)));
			}
			builder.SetPartitionSpecs(std::move(partition_specs));
		}
		auto default_spec_id_val = yyjson_obj_get(obj, "default-spec-id");
		if (default_spec_id_val) {
			int32_t default_spec_id;
			if (yyjson_is_int(default_spec_id_val)) {
				default_spec_id = yyjson_get_int(default_spec_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'default_spec_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(default_spec_id_val)));
			}
			builder.SetDefaultSpecId(std::move(default_spec_id));
		}
		auto last_partition_id_val = yyjson_obj_get(obj, "last-partition-id");
		if (last_partition_id_val) {
			int32_t last_partition_id;
			if (yyjson_is_int(last_partition_id_val)) {
				last_partition_id = yyjson_get_int(last_partition_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'last_partition_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(last_partition_id_val)));
			}
			builder.SetLastPartitionId(std::move(last_partition_id));
		}
		auto sort_orders_val = yyjson_obj_get(obj, "sort-orders");
		if (sort_orders_val) {
			vector<SortOrder> sort_orders;
			if (yyjson_is_arr(sort_orders_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(sort_orders_val, idx, max, val) {
					auto tmp = SortOrder::FromJSON(val);
					sort_orders.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'sort_orders' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(sort_orders_val)));
			}
			builder.SetSortOrders(std::move(sort_orders));
		}
		auto default_sort_order_id_val = yyjson_obj_get(obj, "default-sort-order-id");
		if (default_sort_order_id_val) {
			int32_t default_sort_order_id;
			if (yyjson_is_int(default_sort_order_id_val)) {
				default_sort_order_id = yyjson_get_int(default_sort_order_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'default_sort_order_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(default_sort_order_id_val)));
			}
			builder.SetDefaultSortOrderId(std::move(default_sort_order_id));
		}
		auto encryption_keys_val = yyjson_obj_get(obj, "encryption-keys");
		if (encryption_keys_val) {
			vector<EncryptedKey> encryption_keys;
			if (yyjson_is_arr(encryption_keys_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(encryption_keys_val, idx, max, val) {
					auto tmp = EncryptedKey::FromJSON(val);
					encryption_keys.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'encryption_keys' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(encryption_keys_val)));
			}
			builder.SetEncryptionKeys(std::move(encryption_keys));
		}
		auto snapshots_val = yyjson_obj_get(obj, "snapshots");
		if (snapshots_val) {
			vector<Snapshot> snapshots;
			if (yyjson_is_arr(snapshots_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(snapshots_val, idx, max, val) {
					auto tmp = Snapshot::FromJSON(val);
					snapshots.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TableMetadata property 'snapshots' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(snapshots_val)));
			}
			builder.SetSnapshots(std::move(snapshots));
		}
		auto refs_val = yyjson_obj_get(obj, "refs");
		if (refs_val) {
			builder.SetRefs(SnapshotReferences::FromJSON(refs_val));
		}
		auto current_snapshot_id_val = yyjson_obj_get(obj, "current-snapshot-id");
		if (current_snapshot_id_val) {
			if (yyjson_is_null(current_snapshot_id_val)) {
				//! do nothing, property is explicitly nullable
			} else {
				int64_t current_snapshot_id;
				if (yyjson_is_sint(current_snapshot_id_val)) {
					current_snapshot_id = yyjson_get_sint(current_snapshot_id_val);
				} else if (yyjson_is_uint(current_snapshot_id_val)) {
					current_snapshot_id = yyjson_get_uint(current_snapshot_id_val);
				} else {
					throw InvalidInputException(StringUtil::Format(
					    "TableMetadata property 'current_snapshot_id' is not of type 'integer', found '%s' instead",
					    yyjson_get_type_desc(current_snapshot_id_val)));
				}
				builder.SetCurrentSnapshotId(std::move(current_snapshot_id));
			}
		}
		auto last_sequence_number_val = yyjson_obj_get(obj, "last-sequence-number");
		if (last_sequence_number_val) {
			int64_t last_sequence_number;
			if (yyjson_is_sint(last_sequence_number_val)) {
				last_sequence_number = yyjson_get_sint(last_sequence_number_val);
			} else if (yyjson_is_uint(last_sequence_number_val)) {
				last_sequence_number = yyjson_get_uint(last_sequence_number_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'last_sequence_number' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(last_sequence_number_val)));
			}
			builder.SetLastSequenceNumber(std::move(last_sequence_number));
		}
		auto snapshot_log_val = yyjson_obj_get(obj, "snapshot-log");
		if (snapshot_log_val) {
			builder.SetSnapshotLog(SnapshotLog::FromJSON(snapshot_log_val));
		}
		auto metadata_log_val = yyjson_obj_get(obj, "metadata-log");
		if (metadata_log_val) {
			builder.SetMetadataLog(MetadataLog::FromJSON(metadata_log_val));
		}
		auto statistics_val = yyjson_obj_get(obj, "statistics");
		if (statistics_val) {
			vector<StatisticsFile> statistics;
			if (yyjson_is_arr(statistics_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(statistics_val, idx, max, val) {
					auto tmp = StatisticsFile::FromJSON(val);
					statistics.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TableMetadata property 'statistics' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(statistics_val)));
			}
			builder.SetStatistics(std::move(statistics));
		}
		auto partition_statistics_val = yyjson_obj_get(obj, "partition-statistics");
		if (partition_statistics_val) {
			vector<PartitionStatisticsFile> partition_statistics;
			if (yyjson_is_arr(partition_statistics_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(partition_statistics_val, idx, max, val) {
					auto tmp = PartitionStatisticsFile::FromJSON(val);
					partition_statistics.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "TableMetadata property 'partition_statistics' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(partition_statistics_val)));
			}
			builder.SetPartitionStatistics(std::move(partition_statistics));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableMetadata TableMetadata::FromJSON(yyjson_val *obj) {
	TableMetadataBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

TableMetadata TableMetadata::Copy() const {
	return TableMetadata(*this);
}

string TableMetadata::Validate() const {
	string error;
	if (format_version < 1) {
		return "TableMetadata property 'format-version' must be at least 1";
	}
	if (format_version > 3) {
		return "TableMetadata property 'format-version' must be at most 3";
	}
	if (schemas.has_value()) {
		for (const auto &item : (*schemas)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (partition_specs.has_value()) {
		for (const auto &item : (*partition_specs)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (sort_orders.has_value()) {
		for (const auto &item : (*sort_orders)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (encryption_keys.has_value()) {
		for (const auto &item : (*encryption_keys)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (snapshots.has_value()) {
		for (const auto &item : (*snapshots)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (refs.has_value()) {
		error = (*refs).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (snapshot_log.has_value()) {
		error = (*snapshot_log).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (metadata_log.has_value()) {
		error = (*metadata_log).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (statistics.has_value()) {
		for (const auto &item : (*statistics)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (partition_statistics.has_value()) {
		for (const auto &item : (*partition_statistics)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void TableMetadata::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: format-version
	yyjson_mut_obj_add_int(doc, obj, "format-version", format_version);

	// Serialize: table-uuid
	yyjson_mut_obj_add_strcpy(doc, obj, "table-uuid", table_uuid.c_str());

	// Serialize: location
	if (location.has_value()) {
		auto &location_value = *location;
		yyjson_mut_obj_add_strcpy(doc, obj, "location", location_value.c_str());
	}

	// Serialize: last-updated-ms
	if (last_updated_ms.has_value()) {
		auto &last_updated_ms_value = *last_updated_ms;
		yyjson_mut_obj_add_sint(doc, obj, "last-updated-ms", last_updated_ms_value);
	}

	// Serialize: next-row-id
	if (next_row_id.has_value()) {
		auto &next_row_id_value = *next_row_id;
		yyjson_mut_obj_add_sint(doc, obj, "next-row-id", next_row_id_value);
	}

	// Serialize: properties
	if (properties.has_value()) {
		auto &properties_value = *properties;
		yyjson_mut_val *properties_value_obj = yyjson_mut_obj(doc);
		for (const auto &it : properties_value) {
			auto &key = it.first;
			auto &value = it.second;
			auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
			yyjson_mut_obj_add_strcpy(doc, properties_value_obj, key_ptr, value.c_str());
		}
		yyjson_mut_obj_add_val(doc, obj, "properties", properties_value_obj);
	}

	// Serialize: schemas
	if (schemas.has_value()) {
		auto &schemas_value = *schemas;
		yyjson_mut_val *schemas_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : schemas_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(schemas_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "schemas", schemas_value_arr);
	}

	// Serialize: current-schema-id
	if (current_schema_id.has_value()) {
		auto &current_schema_id_value = *current_schema_id;
		yyjson_mut_obj_add_int(doc, obj, "current-schema-id", current_schema_id_value);
	}

	// Serialize: last-column-id
	if (last_column_id.has_value()) {
		auto &last_column_id_value = *last_column_id;
		yyjson_mut_obj_add_int(doc, obj, "last-column-id", last_column_id_value);
	}

	// Serialize: partition-specs
	if (partition_specs.has_value()) {
		auto &partition_specs_value = *partition_specs;
		yyjson_mut_val *partition_specs_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : partition_specs_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(partition_specs_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "partition-specs", partition_specs_value_arr);
	}

	// Serialize: default-spec-id
	if (default_spec_id.has_value()) {
		auto &default_spec_id_value = *default_spec_id;
		yyjson_mut_obj_add_int(doc, obj, "default-spec-id", default_spec_id_value);
	}

	// Serialize: last-partition-id
	if (last_partition_id.has_value()) {
		auto &last_partition_id_value = *last_partition_id;
		yyjson_mut_obj_add_int(doc, obj, "last-partition-id", last_partition_id_value);
	}

	// Serialize: sort-orders
	if (sort_orders.has_value()) {
		auto &sort_orders_value = *sort_orders;
		yyjson_mut_val *sort_orders_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : sort_orders_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(sort_orders_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "sort-orders", sort_orders_value_arr);
	}

	// Serialize: default-sort-order-id
	if (default_sort_order_id.has_value()) {
		auto &default_sort_order_id_value = *default_sort_order_id;
		yyjson_mut_obj_add_int(doc, obj, "default-sort-order-id", default_sort_order_id_value);
	}

	// Serialize: encryption-keys
	if (encryption_keys.has_value()) {
		auto &encryption_keys_value = *encryption_keys;
		yyjson_mut_val *encryption_keys_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : encryption_keys_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(encryption_keys_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "encryption-keys", encryption_keys_value_arr);
	}

	// Serialize: snapshots
	if (snapshots.has_value()) {
		auto &snapshots_value = *snapshots;
		yyjson_mut_val *snapshots_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : snapshots_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(snapshots_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "snapshots", snapshots_value_arr);
	}

	// Serialize: refs
	if (refs.has_value()) {
		auto &refs_value = *refs;
		yyjson_mut_val *refs_value_val = refs_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "refs", refs_value_val);
	}

	// Serialize: current-snapshot-id
	if (current_snapshot_id.has_value()) {
		auto &current_snapshot_id_value = *current_snapshot_id;
		yyjson_mut_obj_add_sint(doc, obj, "current-snapshot-id", current_snapshot_id_value);
	}

	// Serialize: last-sequence-number
	if (last_sequence_number.has_value()) {
		auto &last_sequence_number_value = *last_sequence_number;
		yyjson_mut_obj_add_sint(doc, obj, "last-sequence-number", last_sequence_number_value);
	}

	// Serialize: snapshot-log
	if (snapshot_log.has_value()) {
		auto &snapshot_log_value = *snapshot_log;
		yyjson_mut_val *snapshot_log_value_val = snapshot_log_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "snapshot-log", snapshot_log_value_val);
	}

	// Serialize: metadata-log
	if (metadata_log.has_value()) {
		auto &metadata_log_value = *metadata_log;
		yyjson_mut_val *metadata_log_value_val = metadata_log_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "metadata-log", metadata_log_value_val);
	}

	// Serialize: statistics
	if (statistics.has_value()) {
		auto &statistics_value = *statistics;
		yyjson_mut_val *statistics_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : statistics_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(statistics_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "statistics", statistics_value_arr);
	}

	// Serialize: partition-statistics
	if (partition_statistics.has_value()) {
		auto &partition_statistics_value = *partition_statistics;
		yyjson_mut_val *partition_statistics_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : partition_statistics_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(partition_statistics_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "partition-statistics", partition_statistics_value_arr);
	}
}

yyjson_mut_val *TableMetadata::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
