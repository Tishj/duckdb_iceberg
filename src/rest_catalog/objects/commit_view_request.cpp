
#include "rest_catalog/objects/commit_view_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

CommitViewRequest::CommitViewRequest(vector<ViewUpdate> updates_p, optional<TableIdentifier> identifier_p,
                                     optional<vector<ViewRequirement>> requirements_p)
    : updates(std::move(updates_p)), identifier(std::move(identifier_p)), requirements(std::move(requirements_p)) {
}

CommitViewRequestBuilder::CommitViewRequestBuilder() {
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetUpdates(vector<ViewUpdate> value) {
	updates_ = std::move(value);
	has_updates_ = true;
	return *this;
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetIdentifier(TableIdentifier value) {
	identifier_ = std::move(value);
	return *this;
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetRequirements(vector<ViewRequirement> value) {
	requirements_ = std::move(value);
	return *this;
}

CommitViewRequest CommitViewRequestBuilder::Build() {
	if (!has_updates_) {
		throw InvalidInputException("CommitViewRequest required property 'updates' is missing");
	}
	auto result = CommitViewRequest(std::move(*updates_), std::move(identifier_), std::move(requirements_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CommitViewRequestBuilder::TryBuild(optional<CommitViewRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitViewRequest CommitViewRequest::FromJSON(yyjson_val *obj) {
	CommitViewRequestBuilder builder;
	auto updates_val = yyjson_obj_get(obj, "updates");
	if (!updates_val) {
		throw InvalidInputException("CommitViewRequest required property 'updates' is missing");
	} else {
		vector<ViewUpdate> updates;
		if (yyjson_is_arr(updates_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(updates_val, idx, max, val) {
				auto tmp = ViewUpdate::FromJSON(val);
				updates.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("CommitViewRequest property 'updates' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(updates_val));
		}
		builder.SetUpdates(std::move(updates));
	}
	auto identifier_val = yyjson_obj_get(obj, "identifier");
	if (identifier_val) {
		optional<TableIdentifier> identifier;
		identifier = TableIdentifier::FromJSON(identifier_val);
		builder.SetIdentifier(std::move(*identifier));
	}
	auto requirements_val = yyjson_obj_get(obj, "requirements");
	if (requirements_val) {
		vector<ViewRequirement> requirements;
		if (yyjson_is_arr(requirements_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(requirements_val, idx, max, val) {
				auto tmp = ViewRequirement::FromJSON(val);
				requirements.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "CommitViewRequest property 'requirements' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(requirements_val));
		}
		builder.SetRequirements(std::move(requirements));
	}
	return builder.Build();
}

string CommitViewRequest::TryFromJSON(yyjson_val *obj, optional<CommitViewRequest> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitViewRequest CommitViewRequest::Copy() const {
	CommitViewRequestBuilder builder;
	vector<ViewUpdate> updates_tmp;
	updates_tmp.reserve(updates.size());
	for (auto &item : updates) {
		updates_tmp.emplace_back(item.Copy());
	}
	builder.SetUpdates(std::move(updates_tmp));
	optional<TableIdentifier> identifier_tmp;
	if (identifier.has_value()) {
		identifier_tmp.emplace();
		(*identifier_tmp) = (*identifier).Copy();
	}
	if (identifier_tmp.has_value()) {
		builder.SetIdentifier(std::move(*identifier_tmp));
	}
	vector<ViewRequirement> requirements_tmp;
	if (requirements.has_value()) {
		requirements_tmp.emplace();
		(*requirements_tmp).reserve((*requirements).size());
		for (auto &item : (*requirements)) {
			(*requirements_tmp).emplace_back(item.Copy());
		}
	}
	if (requirements_tmp.has_value()) {
		builder.SetRequirements(std::move(requirements_tmp));
	}
	return builder.Build();
}

string CommitViewRequest::Validate() const {
	string error;
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
	if (requirements.has_value()) {
		for (const auto &item : (*requirements)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void CommitViewRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

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

	// Serialize: requirements
	if (requirements.has_value()) {
		auto &requirements_value = *requirements;
		yyjson_mut_val *requirements_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : requirements_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(requirements_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "requirements", requirements_value_arr);
	}
}

yyjson_mut_val *CommitViewRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
