
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/metrics.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitReportBuilder;

class CommitReport {
public:
	CommitReport(const CommitReport &);
	CommitReport &operator=(const CommitReport &) = delete;
	CommitReport(CommitReport &&);
	CommitReport &operator=(CommitReport &&) = delete;

private:
	friend class CommitReportBuilder;
	CommitReport(string table_name_p, int64_t snapshot_id_p, int64_t sequence_number_p, string operation_p,
	             Metrics metrics_p, optional<case_insensitive_map_t<string>> metadata_p);

public:
	// Deserialization
	static CommitReport FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, CommitReportBuilder &builder);
	string Validate() const;

	// Copy
	CommitReport Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string table_name;
	const int64_t snapshot_id;
	const int64_t sequence_number;
	const string operation;
	const Metrics metrics;
	const optional<case_insensitive_map_t<string>> metadata;
};

class CommitReportBuilder {
public:
	CommitReportBuilder();
	CommitReportBuilder &SetTableName(string value);
	CommitReportBuilder &SetSnapshotId(int64_t value);
	CommitReportBuilder &SetSequenceNumber(int64_t value);
	CommitReportBuilder &SetOperation(string value);
	CommitReportBuilder &SetMetrics(Metrics value);
	CommitReportBuilder &SetMetadata(case_insensitive_map_t<string> value);
	string TryBuild(optional<CommitReport> &result);
	CommitReport Build();

private:
	optional<string> table_name_;
	optional<int64_t> snapshot_id_;
	optional<int64_t> sequence_number_;
	optional<string> operation_;
	optional<Metrics> metrics_;
	optional<case_insensitive_map_t<string>> metadata_;
	bool has_table_name_ = false;
	bool has_snapshot_id_ = false;
	bool has_sequence_number_ = false;
	bool has_operation_ = false;
	bool has_metrics_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
