#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/table_function_catalog_entry.hpp"
#include "duckdb/common/enums/join_type.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/tableref/joinref.hpp"
#include "duckdb/common/enums/joinref_type.hpp"
#include "duckdb/common/enums/tableref_type.hpp"
#include "duckdb/parser/tableref/table_function_ref.hpp"
#include "duckdb/parser/query_node/recursive_cte_node.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/parser/expression/conjunction_expression.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"
#include "duckdb/parser/expression/comparison_expression.hpp"
#include "duckdb/parser/expression/star_expression.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"
#include "duckdb/parser/tableref/emptytableref.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/operator/logical_comparison_join.hpp"
#include "duckdb/common/file_opener.hpp"
#include "duckdb/common/file_system.hpp"
#include "iceberg_metadata.hpp"
#include "iceberg_functions.hpp"
#include "iceberg_utils.hpp"

#include "metadata/iceberg_table_metadata.hpp"

#include <string>
#include <numeric>

namespace duckdb {

struct IcebergManifestsBindData : public TableFunctionData {
	unique_ptr<IcebergTable> iceberg_table;
};

struct IcebergManifestsGlobalTableFunctionState : public GlobalTableFunctionState {
public:
	IcebergManifestsGlobalTableFunctionState() {

	};

	static unique_ptr<GlobalTableFunctionState> Init(ClientContext &context, TableFunctionInitInput &input) {
		return make_uniq<IcebergManifestsGlobalTableFunctionState>();
	}

	idx_t current_manifest_idx = 0;
	idx_t current_manifest_entry_idx = 0;
};

static unique_ptr<FunctionData> IcebergManifestsBind(ClientContext &context, TableFunctionBindInput &input,
                                                    vector<LogicalType> &return_types, vector<string> &names) {
	// return a TableRef that contains the scans for the
	auto ret = make_uniq<IcebergManifestsBindData>();

	FileSystem &fs = FileSystem::GetFileSystem(context);
	auto input_string = input.inputs[0].ToString();
	auto filename = IcebergUtils::GetStorageLocation(context, input_string);

	IcebergOptions options;
	auto &snapshot_lookup = options.snapshot_lookup;

	for (auto &kv : input.named_parameters) {
		auto loption = StringUtil::Lower(kv.first);
		auto &val = kv.second;
		if (loption == "allow_moved_paths") {
			options.allow_moved_paths = BooleanValue::Get(val);
		} else if (loption == "metadata_compression_codec") {
			options.metadata_compression_codec = StringValue::Get(val);
		} else if (loption == "version") {
			options.table_version = StringValue::Get(val);
		} else if (loption == "version_name_format") {
			auto value = StringValue::Get(kv.second);
			auto string_substitutions = IcebergUtils::CountOccurrences(value, "%s");
			if (string_substitutions != 2) {
				throw InvalidInputException(
				    "'version_name_format' has to contain two occurrences of '%s' in it, found %d", "%s",
				    string_substitutions);
			}
			options.version_name_format = value;
		} else if (loption == "snapshot_from_id") {
			if (snapshot_lookup.snapshot_source != SnapshotSource::LATEST) {
				throw InvalidInputException(
				    "Can't use 'snapshot_from_id' in combination with 'snapshot_from_timestamp'");
			}
			snapshot_lookup.snapshot_source = SnapshotSource::FROM_ID;
			snapshot_lookup.snapshot_id = val.GetValue<uint64_t>();
		} else if (loption == "snapshot_from_timestamp") {
			if (snapshot_lookup.snapshot_source != SnapshotSource::LATEST) {
				throw InvalidInputException(
				    "Can't use 'snapshot_from_id' in combination with 'snapshot_from_timestamp'");
			}
			snapshot_lookup.snapshot_source = SnapshotSource::FROM_TIMESTAMP;
			snapshot_lookup.snapshot_timestamp = val.GetValue<timestamp_t>();
		}
	}

	auto iceberg_meta_path = IcebergTableMetadata::GetMetaDataPath(context, filename, fs, options);
	auto table_metadata = IcebergTableMetadata::Parse(iceberg_meta_path, fs, options.metadata_compression_codec);
	auto metadata = IcebergTableMetadata::FromTableMetadata(table_metadata);

	auto snapshot_to_scan = metadata.GetSnapshot(options.snapshot_lookup);

	if (snapshot_to_scan) {
		ret->iceberg_table = IcebergTable::Load(filename, metadata, *snapshot_to_scan, context, options);
	}

	names.emplace_back("manifest_path");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("manifest_length");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("manifest_content_type");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("manifest_sequence_number");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("added_snapshot_id");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("partition_spec_id");
	return_types.emplace_back(LogicalType::INTEGER);

	return std::move(ret);
}

static void AddString(Vector &vec, idx_t index, string_t &&str) {
	FlatVector::GetData<string_t>(vec)[index] = StringVector::AddString(vec, std::move(str));
}

static void IcebergManifestsFunction(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
	auto &bind_data = data.bind_data->Cast<IcebergManifestsBindData>();
	auto &global_state = data.global_state->Cast<IcebergManifestsGlobalTableFunctionState>();

	if (!bind_data.iceberg_table) {
		//! Table is empty
		return;
	}

	idx_t out = 0;
	auto &table_entries = bind_data.iceberg_table->entries;
	for (; global_state.current_manifest_idx < table_entries.size(); global_state.current_manifest_idx++) {
		auto &table_entry = table_entries[global_state.current_manifest_idx];
		if (out >= STANDARD_VECTOR_SIZE) {
			output.SetCardinality(out);
			return;
		}
		auto &manifest = table_entry.manifest;

		idx_t col = 0;
		//! manifest_path
		AddString(output.data[col++], out, string_t(manifest.manifest_path));
		//! manifest_length
		FlatVector::GetData<int64_t>(output.data[col++])[out] = manifest.manifest_length;
		//! manifest_content_type
		AddString(output.data[col++], out, string_t(IcebergManifestListEntry::ContentTypeToString(manifest.content)));
		//! manifest_sequence_number
		FlatVector::GetData<int64_t>(output.data[col++])[out] = manifest.sequence_number;
		//! added_snapshot_id
		FlatVector::GetData<int64_t>(output.data[col++])[out] = manifest.added_snapshot_id;
		//! partition_spec_id
		FlatVector::GetData<int32_t>(output.data[col++])[out] = manifest.partition_spec_id;

		out++;
		global_state.current_manifest_entry_idx = 0;
	}
	output.SetCardinality(out);
}

TableFunctionSet IcebergFunctions::GetIcebergManifestsFunction() {
	TableFunctionSet function_set("iceberg_manifests");
	TableFunction fun(
		{LogicalType::VARCHAR},
		IcebergManifestsFunction,
		IcebergManifestsBind,
		IcebergManifestsGlobalTableFunctionState::Init
	);

	fun.named_parameters["allow_moved_paths"] = LogicalType::BOOLEAN;
	fun.named_parameters["metadata_compression_codec"] = LogicalType::VARCHAR;
	fun.named_parameters["version"] = LogicalType::VARCHAR;
	fun.named_parameters["version_name_format"] = LogicalType::VARCHAR;
	fun.named_parameters["snapshot_from_timestamp"] = LogicalType::TIMESTAMP;
	fun.named_parameters["snapshot_from_id"] = LogicalType::UBIGINT;
	function_set.AddFunction(fun);
	return function_set;
}

} // namespace duckdb
