
#include "rest_catalog/objects/commit_table_request.hpp"

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

CommitTableRequest::CommitTableRequest(vector<TableRequirement> requirements_p, vector<TableUpdate> updates_p,
                                       optional<TableIdentifier> identifier_p)
    : requirements(std::move(requirements_p)), updates(std::move(updates_p)), identifier(std::move(identifier_p)) {
}

CommitTableRequestBuilder::CommitTableRequestBuilder() {
}

CommitTableRequestBuilder &CommitTableRequestBuilder::SetRequirements(vector<TableRequirement> value) {
	requirements_.emplace(std::move(value));
	has_requirements_ = true;
	return *this;
}

CommitTableRequestBuilder &CommitTableRequestBuilder::SetUpdates(vector<TableUpdate> value) {
	updates_.emplace(std::move(value));
	has_updates_ = true;
	return *this;
}

CommitTableRequestBuilder &CommitTableRequestBuilder::SetIdentifier(TableIdentifier value) {
	identifier_.emplace(std::move(value));
	return *this;
}

CommitTableRequest CommitTableRequestBuilder::Build() {
	if (!has_requirements_) {
		throw InvalidInputException("CommitTableRequest required property 'requirements' is missing");
	}
	if (!has_updates_) {
		throw InvalidInputException("CommitTableRequest required property 'updates' is missing");
	}
	auto result = CommitTableRequest(std::move(*requirements_), std::move(*updates_), std::move(identifier_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CommitTableRequestBuilder::TryBuild(optional<CommitTableRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitTableRequest CommitTableRequest::FromJSON(yyjson_val *obj) {
	CommitTableRequestBuilder builder;
	auto requirements_val = yyjson_obj_get(obj, "requirements");
	if (!requirements_val) {
		throw InvalidInputException("CommitTableRequest required property 'requirements' is missing");
	} else {
		vector<TableRequirement> requirements;
		if (yyjson_is_arr(requirements_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(requirements_val, idx, max, val) {
				auto tmp = TableRequirement::FromJSON(val);
				requirements.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "CommitTableRequest property 'requirements' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(requirements_val)));
		}
		builder.SetRequirements(std::move(requirements));
	}
	auto updates_val = yyjson_obj_get(obj, "updates");
	if (!updates_val) {
		throw InvalidInputException("CommitTableRequest required property 'updates' is missing");
	} else {
		vector<TableUpdate> updates;
		if (yyjson_is_arr(updates_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(updates_val, idx, max, val) {
				auto tmp = TableUpdate::FromJSON(val);
				updates.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(
			    StringUtil::Format("CommitTableRequest property 'updates' is not of type 'array', found '%s' instead",
			                       yyjson_get_type_desc(updates_val)));
		}
		builder.SetUpdates(std::move(updates));
	}
	auto identifier_val = yyjson_obj_get(obj, "identifier");
	if (identifier_val) {
		builder.SetIdentifier(TableIdentifier::FromJSON(identifier_val));
	}
	return builder.Build();
}

string CommitTableRequest::TryFromJSON(yyjson_val *obj, optional<CommitTableRequest> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitTableRequest CommitTableRequest::Copy() const {
	CommitTableRequestBuilder builder;
	vector<TableRequirement> requirements_tmp;
	requirements_tmp.reserve(requirements.size());
	for (auto &item : requirements) {
		requirements_tmp.emplace_back(item.Copy());
	}
	builder.SetRequirements(std::move(requirements_tmp));
	vector<TableUpdate> updates_tmp;
	updates_tmp.reserve(updates.size());
	for (auto &item : updates) {
		updates_tmp.emplace_back(item.Copy());
	}
	builder.SetUpdates(std::move(updates_tmp));
	optional<TableIdentifier> identifier_tmp;
	if (identifier.has_value()) {
		identifier_tmp.emplace((*identifier).Copy());
	}
	if (identifier_tmp.has_value()) {
		builder.SetIdentifier(std::move(*identifier_tmp));
	}
	return builder.Build();
}

string CommitTableRequest::Validate() const {
	string error;
	for (const auto &item : requirements) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	for (const auto &item : updates) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (identifier.has_value()) {
		error = (*identifier).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void CommitTableRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: requirements
	yyjson_mut_val *requirements_arr = yyjson_mut_arr(doc);
	for (const auto &item : requirements) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(requirements_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "requirements", requirements_arr);

	// Serialize: updates
	yyjson_mut_val *updates_arr = yyjson_mut_arr(doc);
	for (const auto &item : updates) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(updates_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "updates", updates_arr);

	// Serialize: identifier
	if (identifier.has_value()) {
		auto &identifier_value = *identifier;
		yyjson_mut_val *identifier_value_val = identifier_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "identifier", identifier_value_val);
	}
}

yyjson_mut_val *CommitTableRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
