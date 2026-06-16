
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
FileFormat::FileFormat(const FileFormat &other) : value(other.value) {
}
FileFormat::FileFormat(FileFormat &&other) : FileFormat(static_cast<const FileFormat &>(other)) {
}

optional<string> FileFormat::TryFromJSON(yyjson_val *obj, optional<FileFormat> &result) {
	try {
		string value;
		if (yyjson_is_str(obj)) {
			value = yyjson_get_str(obj);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "FileFormat property 'value' is not of type 'string', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(FileFormat(std::move(value)));
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

FileFormat FileFormat::FromJSON(yyjson_val *obj) {
	optional<FileFormat> result;
	auto error = TryFromJSON(obj, result);
	if (error) {
		throw InvalidInputException(*error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

FileFormat FileFormat::Copy() const {
	return FileFormat(*this);
}

optional<string> FileFormat::Validate() const {
	optional<string> error;
	if (!StringUtil::CIEquals(value, "avro") && !StringUtil::CIEquals(value, "orc") &&
	    !StringUtil::CIEquals(value, "parquet") && !StringUtil::CIEquals(value, "puffin")) {
		return StringUtil::Format("FileFormat property 'value' must be one of [avro, orc, parquet, puffin], not %s",
		                          value);
	}
	return nullopt;
}

yyjson_mut_val *FileFormat::ToJSON(yyjson_mut_doc *doc) const {
	return yyjson_mut_strcpy(doc, value.c_str());
}

} // namespace rest_api_objects
} // namespace duckdb
