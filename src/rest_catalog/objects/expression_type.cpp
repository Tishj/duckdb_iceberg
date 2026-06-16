
#include "rest_catalog/objects/expression_type.hpp"

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

ExpressionType::ExpressionType(string value_p) : value(std::move(value_p)) {
}
ExpressionType::ExpressionType(const ExpressionType &other) : value(other.value) {
}
ExpressionType::ExpressionType(ExpressionType &&other) : ExpressionType(static_cast<const ExpressionType &>(other)) {
}

string ExpressionType::TryFromJSON(yyjson_val *obj, optional<ExpressionType> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("ExpressionType property 'value' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(obj)));
		}
		result.emplace(ExpressionType(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ExpressionType ExpressionType::FromJSON(yyjson_val *obj) {
	optional<ExpressionType> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

ExpressionType ExpressionType::Copy() const {
	return ExpressionType(*this);
}

string ExpressionType::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *ExpressionType::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
