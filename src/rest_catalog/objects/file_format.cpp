
#include "rest_catalog/objects/file_format.hpp"

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

FileFormat::FileFormat(string value_p) : value(std::move(value_p)) {
}

string FileFormat::TryFromJSON(yyjson_val *obj, optional<FileFormat> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "FileFormat property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(FileFormat(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FileFormat FileFormat::FromJSON(yyjson_val *obj) {
	optional<FileFormat> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

FileFormat FileFormat::Copy() const {
	string value_tmp;
	value_tmp = value;
	return FileFormat(std::move(value_tmp));
}

string FileFormat::Validate() const {
	string error;
	if (!StringUtil::CIEquals(value, "avro") && !StringUtil::CIEquals(value, "orc") &&
	    !StringUtil::CIEquals(value, "parquet") && !StringUtil::CIEquals(value, "puffin")) {
		return StringUtil::Format("FileFormat property 'value' must be one of [avro, orc, parquet, puffin], not %s",
		                          value);
	}
	return "";
}

yyjson_mut_val *FileFormat::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
