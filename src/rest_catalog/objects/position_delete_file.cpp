
#include "rest_catalog/objects/position_delete_file.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

PositionDeleteFile::PositionDeleteFile(ContentFile content_file_p, optional<int64_t> content_offset_p,
                                       optional<int64_t> content_size_in_bytes_p)
    : content_file(std::move(content_file_p)), content_offset(std::move(content_offset_p)),
      content_size_in_bytes(std::move(content_size_in_bytes_p)) {
}

PositionDeleteFileBuilder::PositionDeleteFileBuilder() {
}

PositionDeleteFileBuilder &PositionDeleteFileBuilder::SetContentFile(ContentFile value) {
	content_file_ = std::move(value);
	return *this;
}

PositionDeleteFileBuilder &PositionDeleteFileBuilder::SetContentOffset(int64_t value) {
	content_offset_ = std::move(value);
	return *this;
}

PositionDeleteFileBuilder &PositionDeleteFileBuilder::SetContentSizeInBytes(int64_t value) {
	content_size_in_bytes_ = std::move(value);
	return *this;
}

PositionDeleteFile PositionDeleteFileBuilder::Build() {
	auto result =
	    PositionDeleteFile(std::move(*content_file_), std::move(content_offset_), std::move(content_size_in_bytes_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string PositionDeleteFileBuilder::TryBuild(optional<PositionDeleteFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PositionDeleteFile PositionDeleteFile::FromJSON(yyjson_val *obj) {
	PositionDeleteFileBuilder builder;
	builder.SetContentFile(ContentFile::FromJSON(obj));
	auto content_offset_val = yyjson_obj_get(obj, "content-offset");
	if (content_offset_val) {
		int64_t content_offset;
		if (yyjson_is_sint(content_offset_val)) {
			content_offset = yyjson_get_sint(content_offset_val);
		} else if (yyjson_is_uint(content_offset_val)) {
			content_offset = yyjson_get_uint(content_offset_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "PositionDeleteFile property 'content_offset' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(content_offset_val)));
		}
		builder.SetContentOffset(std::move(content_offset));
	}
	auto content_size_in_bytes_val = yyjson_obj_get(obj, "content-size-in-bytes");
	if (content_size_in_bytes_val) {
		int64_t content_size_in_bytes;
		if (yyjson_is_sint(content_size_in_bytes_val)) {
			content_size_in_bytes = yyjson_get_sint(content_size_in_bytes_val);
		} else if (yyjson_is_uint(content_size_in_bytes_val)) {
			content_size_in_bytes = yyjson_get_uint(content_size_in_bytes_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "PositionDeleteFile property 'content_size_in_bytes' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(content_size_in_bytes_val)));
		}
		builder.SetContentSizeInBytes(std::move(content_size_in_bytes));
	}
	return builder.Build();
}

string PositionDeleteFile::TryFromJSON(yyjson_val *obj, optional<PositionDeleteFile> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

PositionDeleteFile PositionDeleteFile::Copy() const {
	PositionDeleteFileBuilder builder;
	optional<ContentFile> content_file_tmp;
	content_file_tmp = content_file.Copy();
	builder.SetContentFile(std::move(*content_file_tmp));
	int64_t content_offset_tmp;
	if (content_offset.has_value()) {
		content_offset_tmp.emplace();
		(*content_offset_tmp) = (*content_offset);
	}
	if (content_offset_tmp.has_value()) {
		builder.SetContentOffset(std::move(content_offset_tmp));
	}
	int64_t content_size_in_bytes_tmp;
	if (content_size_in_bytes.has_value()) {
		content_size_in_bytes_tmp.emplace();
		(*content_size_in_bytes_tmp) = (*content_size_in_bytes);
	}
	if (content_size_in_bytes_tmp.has_value()) {
		builder.SetContentSizeInBytes(std::move(content_size_in_bytes_tmp));
	}
	return builder.Build();
}

string PositionDeleteFile::Validate() const {
	string error;
	error = content_file.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void PositionDeleteFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: ContentFile
	content_file.PopulateJSON(doc, obj);

	// Serialize: content-offset
	if (content_offset.has_value()) {
		auto &content_offset_value = *content_offset;
		yyjson_mut_obj_add_sint(doc, obj, "content-offset", content_offset_value);
	}

	// Serialize: content-size-in-bytes
	if (content_size_in_bytes.has_value()) {
		auto &content_size_in_bytes_value = *content_size_in_bytes;
		yyjson_mut_obj_add_sint(doc, obj, "content-size-in-bytes", content_size_in_bytes_value);
	}
}

yyjson_mut_val *PositionDeleteFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
