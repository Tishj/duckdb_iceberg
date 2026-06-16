
#include "rest_catalog/objects/list_tables_response.hpp"

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

ListTablesResponse::ListTablesResponse(optional<PageToken> next_page_token_p,
                                       optional<vector<TableIdentifier>> identifiers_p)
    : next_page_token(std::move(next_page_token_p)), identifiers(std::move(identifiers_p)) {
}
ListTablesResponse::ListTablesResponse(const ListTablesResponse &other)
    : next_page_token((other.next_page_token.has_value() ? optional<PageToken>((*other.next_page_token).Copy())
                                                         : optional<PageToken>())),
      identifiers((other.identifiers.has_value() ? optional<vector<TableIdentifier>>(([&]() {
	      vector<TableIdentifier> copied;
	      copied.reserve((*other.identifiers).size());
	      for (const auto &item : (*other.identifiers)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                                 : optional<vector<TableIdentifier>>())) {
}
ListTablesResponse::ListTablesResponse(ListTablesResponse &&other)
    : ListTablesResponse(static_cast<const ListTablesResponse &>(other)) {
}

ListTablesResponseBuilder::ListTablesResponseBuilder() {
}

ListTablesResponseBuilder &ListTablesResponseBuilder::SetNextPageToken(PageToken value) {
	next_page_token_.emplace(std::move(value));
	return *this;
}

ListTablesResponseBuilder &ListTablesResponseBuilder::SetIdentifiers(vector<TableIdentifier> value) {
	identifiers_.emplace(std::move(value));
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

string ListTablesResponse::TryFromJSON(yyjson_val *obj, ListTablesResponseBuilder &builder) {
	try {
		auto next_page_token_val = yyjson_obj_get(obj, "next-page-token");
		if (next_page_token_val) {
			builder.SetNextPageToken(PageToken::FromJSON(next_page_token_val));
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
				throw InvalidInputException(StringUtil::Format(
				    "ListTablesResponse property 'identifiers' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(identifiers_val)));
			}
			builder.SetIdentifiers(std::move(identifiers));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListTablesResponse ListTablesResponse::FromJSON(yyjson_val *obj) {
	ListTablesResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

ListTablesResponse ListTablesResponse::Copy() const {
	return ListTablesResponse(*this);
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
