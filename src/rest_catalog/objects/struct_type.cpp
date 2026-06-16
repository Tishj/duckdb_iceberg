
#include "rest_catalog/objects/struct_type.hpp"

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

StructType::StructType(string type_p, vector<unique_ptr<StructField>> fields_p)
    : type(std::move(type_p)), fields(std::move(fields_p)) {
}

StructTypeBuilder::StructTypeBuilder() {
}

StructTypeBuilder &StructTypeBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

StructTypeBuilder &StructTypeBuilder::SetFields(vector<unique_ptr<StructField>> value) {
	fields_.emplace(std::move(value));
	has_fields_ = true;
	return *this;
}

StructType StructTypeBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("StructType required property 'type' is missing");
	}
	if (!has_fields_) {
		throw InvalidInputException("StructType required property 'fields' is missing");
	}
	auto result = StructType(std::move(*type_), std::move(*fields_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string StructTypeBuilder::TryBuild(optional<StructType> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string StructType::TryFromJSON(yyjson_val *obj, StructTypeBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("StructType required property 'type' is missing");
		} else {
			string type;
			if (yyjson_is_str(type_val)) {
				type = yyjson_get_str(type_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructType property 'type' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(type_val)));
			}
			builder.SetType(std::move(type));
		}
		auto fields_val = yyjson_obj_get(obj, "fields");
		if (!fields_val) {
			throw InvalidInputException("StructType required property 'fields' is missing");
		} else {
			vector<unique_ptr<StructField>> fields;
			if (yyjson_is_arr(fields_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(fields_val, idx, max, val) {
					auto tmp = make_uniq<StructField>(StructField::FromJSON(val));
					fields.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("StructType property 'fields' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(fields_val)));
			}
			builder.SetFields(std::move(fields));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

StructType StructType::FromJSON(yyjson_val *obj) {
	StructTypeBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

StructType StructType::Copy() const {
	StructTypeBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	vector<unique_ptr<StructField>> fields_tmp;
	fields_tmp.reserve(fields.size());
	for (auto &item : fields) {
		fields_tmp.emplace_back(item ? make_uniq<StructField>(item->Copy()) : nullptr);
	}
	builder.SetFields(std::move(fields_tmp));
	return builder.Build();
}

string StructType::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "struct")) {
		return StringUtil::Format("StructType property 'type' must be struct, not %s", type);
	}
	for (const auto &item : fields) {
		error = item->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void StructType::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: fields
	yyjson_mut_val *fields_arr = yyjson_mut_arr(doc);
	for (const auto &item : fields) {
		yyjson_mut_val *item_val = item->ToJSON(doc);
		yyjson_mut_arr_append(fields_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "fields", fields_arr);
}

yyjson_mut_val *StructType::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
