
#include "rest_catalog/objects/commit_view_request.hpp"

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

CommitViewRequest::CommitViewRequest(vector<ViewUpdate> updates_p, optional<TableIdentifier> identifier_p,
                                     optional<vector<ViewRequirement>> requirements_p)
    : updates(std::move(updates_p)), identifier(std::move(identifier_p)), requirements(std::move(requirements_p)) {
}
CommitViewRequest::CommitViewRequest(const CommitViewRequest &other)
    : updates(([&]() {
	      vector<ViewUpdate> copied;
	      copied.reserve(other.updates.size());
	      for (const auto &item : other.updates) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }())),
      identifier((other.identifier.has_value() ? optional<TableIdentifier>((*other.identifier).Copy())
                                               : optional<TableIdentifier>())),
      requirements((other.requirements.has_value() ? optional<vector<ViewRequirement>>(([&]() {
	      vector<ViewRequirement> copied;
	      copied.reserve((*other.requirements).size());
	      for (const auto &item : (*other.requirements)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                   : optional<vector<ViewRequirement>>())) {
}
CommitViewRequest::CommitViewRequest(CommitViewRequest &&other)
    : CommitViewRequest(static_cast<const CommitViewRequest &>(other)) {
}

CommitViewRequestBuilder::CommitViewRequestBuilder() {
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetUpdates(vector<ViewUpdate> value) {
	updates_.emplace(std::move(value));
	has_updates_ = true;
	return *this;
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetIdentifier(TableIdentifier value) {
	identifier_.emplace(std::move(value));
	return *this;
}

CommitViewRequestBuilder &CommitViewRequestBuilder::SetRequirements(vector<ViewRequirement> value) {
	requirements_.emplace(std::move(value));
	return *this;
}

CommitViewRequest CommitViewRequestBuilder::Build() {
	if (!has_updates_) {
		throw InvalidInputException("CommitViewRequest required property 'updates' is missing");
	}
	auto result = CommitViewRequest(std::move(*updates_), std::move(identifier_), std::move(requirements_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> CommitViewRequestBuilder::TryBuild(optional<CommitViewRequest> &result) {
	if (!has_updates_) {
		return "CommitViewRequest required property 'updates' is missing";
	}
	auto built = CommitViewRequest(std::move(*updates_), std::move(identifier_), std::move(requirements_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> CommitViewRequest::TryFromJSON(yyjson_val *obj, CommitViewRequestBuilder &builder) {
	try {
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
				throw InvalidInputException(StringUtil::Format(
				    "CommitViewRequest property 'updates' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(updates_val)));
			}
			builder.SetUpdates(std::move(updates));
		}
		auto identifier_val = yyjson_obj_get(obj, "identifier");
		if (identifier_val) {
			builder.SetIdentifier(TableIdentifier::FromJSON(identifier_val));
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
				throw InvalidInputException(StringUtil::Format(
				    "CommitViewRequest property 'requirements' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(requirements_val)));
			}
			builder.SetRequirements(std::move(requirements));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitViewRequest CommitViewRequest::FromJSON(yyjson_val *obj) {
	CommitViewRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

CommitViewRequest CommitViewRequest::Copy() const {
	return CommitViewRequest(*this);
}

optional<string> CommitViewRequest::Validate() const {
	optional<string> error;
	for (const auto &item : updates) {
		error = item.Validate();
		if (error) {
			return error;
		}
	}
	if (identifier.has_value()) {
		error = (*identifier).Validate();
		if (error) {
			return error;
		}
	}
	if (requirements.has_value()) {
		for (const auto &item : (*requirements)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	return nullopt;
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
