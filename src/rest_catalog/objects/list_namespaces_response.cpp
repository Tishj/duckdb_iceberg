
#include "rest_catalog/objects/list_namespaces_response.hpp"

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

ListNamespacesResponse::ListNamespacesResponse(optional<PageToken> next_page_token_p,
                                               optional<vector<Namespace>> namespaces_p)
    : next_page_token(std::move(next_page_token_p)), namespaces(std::move(namespaces_p)) {
}

ListNamespacesResponseBuilder::ListNamespacesResponseBuilder() {
}

ListNamespacesResponseBuilder &ListNamespacesResponseBuilder::SetNextPageToken(PageToken value) {
	next_page_token_.emplace(std::move(value));
	return *this;
}

ListNamespacesResponseBuilder &ListNamespacesResponseBuilder::SetNamespaces(vector<Namespace> value) {
	namespaces_.emplace(std::move(value));
	return *this;
}

ListNamespacesResponse ListNamespacesResponseBuilder::Build() {
	auto result = ListNamespacesResponse(std::move(next_page_token_), std::move(namespaces_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ListNamespacesResponseBuilder::TryBuild(optional<ListNamespacesResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListNamespacesResponse ListNamespacesResponse::FromJSON(yyjson_val *obj) {
	ListNamespacesResponseBuilder builder;
	auto next_page_token_val = yyjson_obj_get(obj, "next-page-token");
	if (next_page_token_val) {
		builder.SetNextPageToken(PageToken::FromJSON(next_page_token_val));
	}
	auto namespaces_val = yyjson_obj_get(obj, "namespaces");
	if (namespaces_val) {
		vector<Namespace> namespaces;
		if (yyjson_is_arr(namespaces_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(namespaces_val, idx, max, val) {
				auto tmp = Namespace::FromJSON(val);
				namespaces.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "ListNamespacesResponse property 'namespaces' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(namespaces_val)));
		}
		builder.SetNamespaces(std::move(namespaces));
	}
	return builder.Build();
}

string ListNamespacesResponse::TryFromJSON(yyjson_val *obj, optional<ListNamespacesResponse> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ListNamespacesResponse ListNamespacesResponse::Copy() const {
	ListNamespacesResponseBuilder builder;
	optional<PageToken> next_page_token_tmp;
	if (next_page_token.has_value()) {
		next_page_token_tmp.emplace((*next_page_token).Copy());
	}
	if (next_page_token_tmp.has_value()) {
		builder.SetNextPageToken(std::move(*next_page_token_tmp));
	}
	optional<vector<Namespace>> namespaces_tmp;
	if (namespaces.has_value()) {
		namespaces_tmp.emplace();
		(*namespaces_tmp).reserve((*namespaces).size());
		for (auto &item : (*namespaces)) {
			(*namespaces_tmp).emplace_back(item.Copy());
		}
	}
	if (namespaces_tmp.has_value()) {
		builder.SetNamespaces(std::move((*namespaces_tmp)));
	}
	return builder.Build();
}

string ListNamespacesResponse::Validate() const {
	string error;
	if (next_page_token.has_value()) {
		error = (*next_page_token).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (namespaces.has_value()) {
		for (const auto &item : (*namespaces)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
}

void ListNamespacesResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: next-page-token
	if (next_page_token.has_value()) {
		auto &next_page_token_value = *next_page_token;
		yyjson_mut_val *next_page_token_value_val = next_page_token_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "next-page-token", next_page_token_value_val);
	}

	// Serialize: namespaces
	if (namespaces.has_value()) {
		auto &namespaces_value = *namespaces;
		yyjson_mut_val *namespaces_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : namespaces_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(namespaces_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "namespaces", namespaces_value_arr);
	}
}

yyjson_mut_val *ListNamespacesResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
