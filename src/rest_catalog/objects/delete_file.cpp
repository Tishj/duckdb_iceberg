
#include "rest_catalog/objects/delete_file.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

DeleteFile::DeleteFile()
    : position_delete_file(GeneratedObjectAccess::Create<optional<PositionDeleteFile>>()),
      equality_delete_file(GeneratedObjectAccess::Create<optional<EqualityDeleteFile>>()) {
}

DeleteFileBuilder::DeleteFileBuilder() {
}

DeleteFileBuilder &DeleteFileBuilder::SetPositionDeleteFile(PositionDeleteFile value) {
	result_.position_delete_file = std::move(value);
	return *this;
}

DeleteFileBuilder &DeleteFileBuilder::SetEqualityDeleteFile(EqualityDeleteFile value) {
	result_.equality_delete_file = std::move(value);
	return *this;
}

string DeleteFileBuilder::TryBuild(DeleteFile &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

DeleteFile DeleteFileBuilder::Build() {
	DeleteFile result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

DeleteFile DeleteFile::FromJSON(yyjson_val *obj) {
	DeleteFile res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

DeleteFile DeleteFile::Copy() const {
	DeleteFile res;
	if (position_delete_file.has_value()) {
		res.position_delete_file = GeneratedObjectAccess::Create<PositionDeleteFile>();
		(*res.position_delete_file) = (*position_delete_file).Copy();
	}
	if (equality_delete_file.has_value()) {
		res.equality_delete_file = GeneratedObjectAccess::Create<EqualityDeleteFile>();
		(*res.equality_delete_file) = (*equality_delete_file).Copy();
	}
	return res;
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

string DeleteFile::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		position_delete_file = GeneratedObjectAccess::Create<PositionDeleteFile>();
		error = position_delete_file->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			position_delete_file = nullopt;
		}
		equality_delete_file = GeneratedObjectAccess::Create<EqualityDeleteFile>();
		error = equality_delete_file->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			equality_delete_file = nullopt;
		}
		return "DeleteFile failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
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
