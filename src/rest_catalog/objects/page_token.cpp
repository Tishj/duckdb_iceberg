
#include "rest_catalog/objects/page_token.hpp"

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

PageToken::PageToken(string value_p) : value(std::move(value_p)) {
}

string PageToken::TryFromJSON(yyjson_val *obj, optional<PageToken> &result) {
	try {
		string value;
		if (yyjson_is_null(obj)) {
			//! do nothing, property is explicitly nullable
		} else if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "PageToken property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(PageToken(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PageToken PageToken::FromJSON(yyjson_val *obj) {
	optional<PageToken> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

PageToken PageToken::Copy() const {
	string value_tmp;
	value_tmp = value;
	return PageToken(std::move(value_tmp));
}

string PageToken::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *PageToken::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
