
#include "rest_catalog/objects/sort_direction.hpp"

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

SortDirection::SortDirection(string value_p) : value(std::move(value_p)) {
}

SortDirection SortDirection::FromJSON(yyjson_val *obj) {
	string value;
	if (yyjson_is_str(obj)) {
		value = yyjson_get_str(obj);
	} else {
		throw InvalidInputException(StringUtil::Format(
		    "SortDirection property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
	}
	return SortDirection(std::move(value));
}

string SortDirection::TryFromJSON(yyjson_val *obj, optional<SortDirection> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SortDirection SortDirection::Copy() const {
	string value_tmp;
	value_tmp = value;
	return SortDirection(std::move(value_tmp));
}

string SortDirection::Validate() const {
	string error;
	if (!StringUtil::CIEquals(value, "asc") && !StringUtil::CIEquals(value, "desc")) {
		return StringUtil::Format("SortDirection property 'value' must be one of [asc, desc], not %s", value);
	}
	return "";
}

yyjson_mut_val *SortDirection::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
