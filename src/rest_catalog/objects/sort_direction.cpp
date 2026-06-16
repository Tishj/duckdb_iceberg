
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
SortDirection::SortDirection(const SortDirection &other) : value(other.value) {
}
SortDirection::SortDirection(SortDirection &&other) : SortDirection(static_cast<const SortDirection &>(other)) {
}

optional<string> SortDirection::TryFromJSON(yyjson_val *obj, optional<SortDirection> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SortDirection property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(SortDirection(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SortDirection SortDirection::FromJSON(yyjson_val *obj) {
	optional<SortDirection> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

SortDirection SortDirection::Copy() const {
	return SortDirection(*this);
}

optional<string> SortDirection::Validate() const {
	optional<string> error;
	if (!StringUtil::CIEquals(value, "asc") && !StringUtil::CIEquals(value, "desc")) {
		return StringUtil::Format("SortDirection property 'value' must be one of [asc, desc], not %s", value);
	}
	return nullopt;
}

yyjson_mut_val *SortDirection::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
