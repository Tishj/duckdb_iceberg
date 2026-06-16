
#include "rest_catalog/objects/equality_delete_file.hpp"

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

EqualityDeleteFile::EqualityDeleteFile(ContentFile content_file_p, optional<vector<int32_t>> equality_ids_p)
    : content_file(std::move(content_file_p)), equality_ids(std::move(equality_ids_p)) {
}
EqualityDeleteFile::EqualityDeleteFile(const EqualityDeleteFile &other)
    : content_file(other.content_file.Copy()),
      equality_ids((other.equality_ids.has_value() ? optional<vector<int32_t>>(([&]() {
	      vector<int32_t> copied;
	      copied.reserve((*other.equality_ids).size());
	      for (const auto &item : (*other.equality_ids)) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }()))
                                                   : optional<vector<int32_t>>())) {
}
EqualityDeleteFile::EqualityDeleteFile(EqualityDeleteFile &&other)
    : EqualityDeleteFile(static_cast<const EqualityDeleteFile &>(other)) {
}

EqualityDeleteFileBuilder::EqualityDeleteFileBuilder() {
}

EqualityDeleteFileBuilder &EqualityDeleteFileBuilder::SetContentFile(ContentFile value) {
	content_file_.emplace(std::move(value));
	return *this;
}

EqualityDeleteFileBuilder &EqualityDeleteFileBuilder::SetEqualityIds(vector<int32_t> value) {
	equality_ids_.emplace(std::move(value));
	return *this;
}

EqualityDeleteFile EqualityDeleteFileBuilder::Build() {
	auto result = EqualityDeleteFile(std::move(*content_file_), std::move(equality_ids_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string EqualityDeleteFileBuilder::TryBuild(optional<EqualityDeleteFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string EqualityDeleteFile::TryFromJSON(yyjson_val *obj, EqualityDeleteFileBuilder &builder) {
	try {
		builder.SetContentFile(ContentFile::FromJSON(obj));
		auto equality_ids_val = yyjson_obj_get(obj, "equality-ids");
		if (equality_ids_val) {
			vector<int32_t> equality_ids;
			if (yyjson_is_arr(equality_ids_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(equality_ids_val, idx, max, val) {
					int32_t tmp;
					if (yyjson_is_int(val)) {
						tmp = yyjson_get_int(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "EqualityDeleteFile property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					equality_ids.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "EqualityDeleteFile property 'equality_ids' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(equality_ids_val)));
			}
			builder.SetEqualityIds(std::move(equality_ids));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

EqualityDeleteFile EqualityDeleteFile::FromJSON(yyjson_val *obj) {
	EqualityDeleteFileBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

EqualityDeleteFile EqualityDeleteFile::Copy() const {
	return EqualityDeleteFile(*this);
}

string EqualityDeleteFile::Validate() const {
	string error;
	error = content_file.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void EqualityDeleteFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: ContentFile
	content_file.PopulateJSON(doc, obj);

	// Serialize: equality-ids
	if (equality_ids.has_value()) {
		auto &equality_ids_value = *equality_ids;
		yyjson_mut_val *equality_ids_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : equality_ids_value) {
			yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
			yyjson_mut_arr_append(equality_ids_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "equality-ids", equality_ids_value_arr);
	}
}

yyjson_mut_val *EqualityDeleteFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
