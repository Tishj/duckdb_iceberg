
#include "rest_catalog/objects/delete_file.hpp"

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

DeleteFile::DeleteFile(optional<PositionDeleteFile> position_delete_file_p,
                       optional<EqualityDeleteFile> equality_delete_file_p)
    : position_delete_file(std::move(position_delete_file_p)), equality_delete_file(std::move(equality_delete_file_p)) {
}

DeleteFileBuilder::DeleteFileBuilder() {
}

DeleteFileBuilder &DeleteFileBuilder::SetPositionDeleteFile(PositionDeleteFile value) {
	position_delete_file_.emplace(std::move(value));
	return *this;
}

DeleteFileBuilder &DeleteFileBuilder::SetEqualityDeleteFile(EqualityDeleteFile value) {
	equality_delete_file_.emplace(std::move(value));
	return *this;
}

DeleteFile DeleteFileBuilder::Build() {
	auto result = DeleteFile(std::move(position_delete_file_), std::move(equality_delete_file_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string DeleteFileBuilder::TryBuild(optional<DeleteFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string DeleteFile::TryFromJSON(yyjson_val *obj, DeleteFileBuilder &builder) {
	try {
		do {
			try {
				builder.SetPositionDeleteFile(PositionDeleteFile::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetEqualityDeleteFile(EqualityDeleteFile::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			throw InvalidInputException("DeleteFile failed to parse, none of the oneOf candidates matched");
		} while (false);
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DeleteFile DeleteFile::FromJSON(yyjson_val *obj) {
	DeleteFileBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

DeleteFile DeleteFile::Copy() const {
	DeleteFileBuilder builder;
	optional<PositionDeleteFile> position_delete_file_tmp;
	if (position_delete_file.has_value()) {
		position_delete_file_tmp.emplace((*position_delete_file).Copy());
	}
	if (position_delete_file_tmp.has_value()) {
		builder.SetPositionDeleteFile(std::move(*position_delete_file_tmp));
	}
	optional<EqualityDeleteFile> equality_delete_file_tmp;
	if (equality_delete_file.has_value()) {
		equality_delete_file_tmp.emplace((*equality_delete_file).Copy());
	}
	if (equality_delete_file_tmp.has_value()) {
		builder.SetEqualityDeleteFile(std::move(*equality_delete_file_tmp));
	}
	return builder.Build();
}

string DeleteFile::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (position_delete_file.has_value()) {
		matched_one_of_variants++;
		error = position_delete_file->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (equality_delete_file.has_value()) {
		matched_one_of_variants++;
		error = equality_delete_file->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "DeleteFile must have exactly one oneOf variant set";
	}
	return "";
}

void DeleteFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (position_delete_file.has_value()) {
		position_delete_file->PopulateJSON(doc, obj);
	} else if (equality_delete_file.has_value()) {
		equality_delete_file->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *DeleteFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
