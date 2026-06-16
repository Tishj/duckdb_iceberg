
#include "rest_catalog/objects/commit_transaction_request.hpp"

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

CommitTransactionRequest::CommitTransactionRequest(vector<CommitTableRequest> table_changes_p)
    : table_changes(std::move(table_changes_p)) {
}

CommitTransactionRequestBuilder::CommitTransactionRequestBuilder() {
}

CommitTransactionRequestBuilder &CommitTransactionRequestBuilder::SetTableChanges(vector<CommitTableRequest> value) {
	table_changes_.emplace(std::move(value));
	has_table_changes_ = true;
	return *this;
}

CommitTransactionRequest CommitTransactionRequestBuilder::Build() {
	if (!has_table_changes_) {
		throw InvalidInputException("CommitTransactionRequest required property 'table-changes' is missing");
	}
	auto result = CommitTransactionRequest(std::move(*table_changes_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CommitTransactionRequestBuilder::TryBuild(optional<CommitTransactionRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CommitTransactionRequest::TryFromJSON(yyjson_val *obj, CommitTransactionRequestBuilder &builder) {
	try {
		auto table_changes_val = yyjson_obj_get(obj, "table-changes");
		if (!table_changes_val) {
			throw InvalidInputException("CommitTransactionRequest required property 'table-changes' is missing");
		} else {
			vector<CommitTableRequest> table_changes;
			if (yyjson_is_arr(table_changes_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(table_changes_val, idx, max, val) {
					auto tmp = CommitTableRequest::FromJSON(val);
					table_changes.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "CommitTransactionRequest property 'table_changes' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(table_changes_val)));
			}
			builder.SetTableChanges(std::move(table_changes));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitTransactionRequest CommitTransactionRequest::FromJSON(yyjson_val *obj) {
	CommitTransactionRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CommitTransactionRequest CommitTransactionRequest::Copy() const {
	CommitTransactionRequestBuilder builder;
	vector<CommitTableRequest> table_changes_tmp;
	table_changes_tmp.reserve(table_changes.size());
	for (auto &item : table_changes) {
		table_changes_tmp.emplace_back(item.Copy());
	}
	builder.SetTableChanges(std::move(table_changes_tmp));
	return builder.Build();
}

string CommitTransactionRequest::Validate() const {
	string error;
	for (const auto &item : table_changes) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void CommitTransactionRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: table-changes
	yyjson_mut_val *table_changes_arr = yyjson_mut_arr(doc);
	for (const auto &item : table_changes) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(table_changes_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "table-changes", table_changes_arr);
}

yyjson_mut_val *CommitTransactionRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
