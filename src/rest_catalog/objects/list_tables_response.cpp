
#include "rest_catalog/objects/list_tables_response.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ListTablesResponse::ListTablesResponse(optional<PageToken> next_page_token_p,
                                       optional<vector<TableIdentifier>> identifiers_p)
    : next_page_token(std::move(next_page_token_p)), identifiers(std::move(identifiers_p)) {
}

ListTablesResponseBuilder::ListTablesResponseBuilder() {
}

ListTablesResponseBuilder &ListTablesResponseBuilder::SetNextPageToken(PageToken value) {
	next_page_token_ = std::move(value);
	return *this;
}

ListTablesResponseBuilder &ListTablesResponseBuilder::SetIdentifiers(vector<TableIdentifier> value) {
	identifiers_ = std::move(value);
	return *this;
}

ListTablesResponse ListTablesResponseBuilder::Build() {
	auto result = ListTablesResponse(std::move(next_page_token_), std::move(identifiers_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ListTablesResponseBuilder::TryBuild(optional<ListTablesResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListTablesResponse ListTablesResponse::FromJSON(yyjson_val *obj) {
	ListTablesResponseBuilder builder;
	auto next_page_token_val = yyjson_obj_get(obj, "next-page-token");
	if (next_page_token_val) {
		optional<PageToken> next_page_token;
		next_page_token = PageToken::FromJSON(next_page_token_val);
		builder.SetNextPageToken(std::move(*next_page_token));
	}
	auto identifiers_val = yyjson_obj_get(obj, "identifiers");
	if (identifiers_val) {
		vector<TableIdentifier> identifiers;
		if (yyjson_is_arr(identifiers_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(identifiers_val, idx, max, val) {
				auto tmp = TableIdentifier::FromJSON(val);
				identifiers.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "ListTablesResponse property 'identifiers' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(identifiers_val));
		}
		builder.SetIdentifiers(std::move(identifiers));
	}
	return builder.Build();
}

string ListTablesResponse::TryFromJSON(yyjson_val *obj, optional<ListTablesResponse> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListTablesResponse ListTablesResponse::Copy() const {
	ListTablesResponseBuilder builder;
	optional<PageToken> next_page_token_tmp;
	if (next_page_token.has_value()) {
		next_page_token_tmp.emplace();
		(*next_page_token_tmp) = (*next_page_token).Copy();
	}
	if (next_page_token_tmp.has_value()) {
		builder.SetNextPageToken(std::move(*next_page_token_tmp));
	}
	vector<TableIdentifier> identifiers_tmp;
	if (identifiers.has_value()) {
		identifiers_tmp.emplace();
		(*identifiers_tmp).reserve((*identifiers).size());
		for (auto &item : (*identifiers)) {
			(*identifiers_tmp).emplace_back(item.Copy());
		}
	}
	if (identifiers_tmp.has_value()) {
		builder.SetIdentifiers(std::move(identifiers_tmp));
	}
	return builder.Build();
}

string ListTablesResponse::Validate() const {
	string error;
	if (next_page_token.has_value()) {
		error = (*next_page_token).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (identifiers.has_value()) {
		for (const auto &item : (*identifiers)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void ListTablesResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: next-page-token
	if (next_page_token.has_value()) {
		auto &next_page_token_value = *next_page_token;
		yyjson_mut_val *next_page_token_value_val = next_page_token_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "next-page-token", next_page_token_value_val);
	}

	// Serialize: identifiers
	if (identifiers.has_value()) {
		auto &identifiers_value = *identifiers;
		yyjson_mut_val *identifiers_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : identifiers_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(identifiers_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "identifiers", identifiers_value_arr);
	}
}

yyjson_mut_val *ListTablesResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
