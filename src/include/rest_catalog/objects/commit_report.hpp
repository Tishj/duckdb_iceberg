
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/metrics.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class CommitReport {
public:
	CommitReport();
	CommitReport(const CommitReport &) = delete;
	CommitReport &operator=(const CommitReport &) = delete;
	CommitReport(CommitReport &&) = default;
	CommitReport &operator=(CommitReport &&) = default;

public:
	// Deserialization
	static CommitReport FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
	string Validate() const;

	// Copy
	CommitReport Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	string table_name;
	int64_t snapshot_id;
	int64_t sequence_number;
	string operation;
	Metrics metrics;
	optional<case_insensitive_map_t<string>> metadata;
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
	string TryBuild(CommitReport &result);
	CommitReport Build();

private:
	CommitReport result_;
	bool has_table_name_ = false;
	bool has_snapshot_id_ = false;
	bool has_sequence_number_ = false;
	bool has_operation_ = false;
	bool has_metrics_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
