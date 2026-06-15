
#include "rest_catalog/objects/data_file.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

DataFile::DataFile() {
}

DataFileBuilder::DataFileBuilder() {
}

DataFileBuilder &DataFileBuilder::SetContentFile(ContentFile value) {
	result_.content_file = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetFirstRowId(int64_t value) {
	result_.first_row_id = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetColumnSizes(CountMap value) {
	result_.column_sizes = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetValueCounts(CountMap value) {
	result_.value_counts = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetNullValueCounts(CountMap value) {
	result_.null_value_counts = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetNanValueCounts(CountMap value) {
	result_.nan_value_counts = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetLowerBounds(ValueMap value) {
	result_.lower_bounds = std::move(value);
	return *this;
}

DataFileBuilder &DataFileBuilder::SetUpperBounds(ValueMap value) {
	result_.upper_bounds = std::move(value);
	return *this;
}

string DataFileBuilder::TryBuild(DataFile &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

DataFile DataFileBuilder::Build() {
	DataFile result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

DataFile DataFile::FromJSON(yyjson_val *obj) {
	DataFile res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

DataFile DataFile::Copy() const {
	DataFile res;
	res.content_file = content_file.Copy();
	if (first_row_id.has_value()) {
		res.first_row_id.emplace();
		(*res.first_row_id) = (*first_row_id);
	}
	if (column_sizes.has_value()) {
		res.column_sizes.emplace();
		(*res.column_sizes) = (*column_sizes).Copy();
	}
	if (value_counts.has_value()) {
		res.value_counts.emplace();
		(*res.value_counts) = (*value_counts).Copy();
	}
	if (null_value_counts.has_value()) {
		res.null_value_counts.emplace();
		(*res.null_value_counts) = (*null_value_counts).Copy();
	}
	if (nan_value_counts.has_value()) {
		res.nan_value_counts.emplace();
		(*res.nan_value_counts) = (*nan_value_counts).Copy();
	}
	if (lower_bounds.has_value()) {
		res.lower_bounds.emplace();
		(*res.lower_bounds) = (*lower_bounds).Copy();
	}
	if (upper_bounds.has_value()) {
		res.upper_bounds.emplace();
		(*res.upper_bounds) = (*upper_bounds).Copy();
	}
	return res;
}

string DataFile::Validate() const {
	string error;
	error = content_file.Validate();
	if (!error.empty()) {
		return error;
	}
	if (column_sizes.has_value()) {
		error = (*column_sizes).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (value_counts.has_value()) {
		error = (*value_counts).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (null_value_counts.has_value()) {
		error = (*null_value_counts).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (nan_value_counts.has_value()) {
		error = (*nan_value_counts).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (lower_bounds.has_value()) {
		error = (*lower_bounds).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (upper_bounds.has_value()) {
		error = (*upper_bounds).Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

string DataFile::TryFromJSON(yyjson_val *obj) {
	string error;
	error = content_file.TryFromJSON(obj);
	if (!error.empty()) {
		return error;
	}
	auto first_row_id_val = yyjson_obj_get(obj, "first-row-id");
	if (first_row_id_val) {
		int64_t first_row_id_tmp;
		if (yyjson_is_sint(first_row_id_val)) {
			first_row_id_tmp = yyjson_get_sint(first_row_id_val);
		} else if (yyjson_is_uint(first_row_id_val)) {
			first_row_id_tmp = yyjson_get_uint(first_row_id_val);
		} else {
			return StringUtil::Format(
			    "DataFile property 'first_row_id_tmp' is not of type 'integer', found '%s' instead",
			    yyjson_get_type_desc(first_row_id_val));
		}
		first_row_id = std::move(first_row_id_tmp);
	}
	auto column_sizes_val = yyjson_obj_get(obj, "column-sizes");
	if (column_sizes_val) {
		CountMap column_sizes_tmp;
		error = column_sizes_tmp.TryFromJSON(column_sizes_val);
		if (!error.empty()) {
			return error;
		}
		column_sizes = std::move(column_sizes_tmp);
	}
	auto value_counts_val = yyjson_obj_get(obj, "value-counts");
	if (value_counts_val) {
		CountMap value_counts_tmp;
		error = value_counts_tmp.TryFromJSON(value_counts_val);
		if (!error.empty()) {
			return error;
		}
		value_counts = std::move(value_counts_tmp);
	}
	auto null_value_counts_val = yyjson_obj_get(obj, "null-value-counts");
	if (null_value_counts_val) {
		CountMap null_value_counts_tmp;
		error = null_value_counts_tmp.TryFromJSON(null_value_counts_val);
		if (!error.empty()) {
			return error;
		}
		null_value_counts = std::move(null_value_counts_tmp);
	}
	auto nan_value_counts_val = yyjson_obj_get(obj, "nan-value-counts");
	if (nan_value_counts_val) {
		CountMap nan_value_counts_tmp;
		error = nan_value_counts_tmp.TryFromJSON(nan_value_counts_val);
		if (!error.empty()) {
			return error;
		}
		nan_value_counts = std::move(nan_value_counts_tmp);
	}
	auto lower_bounds_val = yyjson_obj_get(obj, "lower-bounds");
	if (lower_bounds_val) {
		ValueMap lower_bounds_tmp;
		error = lower_bounds_tmp.TryFromJSON(lower_bounds_val);
		if (!error.empty()) {
			return error;
		}
		lower_bounds = std::move(lower_bounds_tmp);
	}
	auto upper_bounds_val = yyjson_obj_get(obj, "upper-bounds");
	if (upper_bounds_val) {
		ValueMap upper_bounds_tmp;
		error = upper_bounds_tmp.TryFromJSON(upper_bounds_val);
		if (!error.empty()) {
			return error;
		}
		upper_bounds = std::move(upper_bounds_tmp);
	}
	return Validate();
}

void DataFile::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: ContentFile
	content_file.PopulateJSON(doc, obj);

	// Serialize: first-row-id
	if (first_row_id.has_value()) {
		auto &first_row_id_value = *first_row_id;
		yyjson_mut_obj_add_sint(doc, obj, "first-row-id", first_row_id_value);
	}

	// Serialize: column-sizes
	if (column_sizes.has_value()) {
		auto &column_sizes_value = *column_sizes;
		yyjson_mut_val *column_sizes_value_val = column_sizes_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "column-sizes", column_sizes_value_val);
	}

	// Serialize: value-counts
	if (value_counts.has_value()) {
		auto &value_counts_value = *value_counts;
		yyjson_mut_val *value_counts_value_val = value_counts_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "value-counts", value_counts_value_val);
	}

	// Serialize: null-value-counts
	if (null_value_counts.has_value()) {
		auto &null_value_counts_value = *null_value_counts;
		yyjson_mut_val *null_value_counts_value_val = null_value_counts_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "null-value-counts", null_value_counts_value_val);
	}

	// Serialize: nan-value-counts
	if (nan_value_counts.has_value()) {
		auto &nan_value_counts_value = *nan_value_counts;
		yyjson_mut_val *nan_value_counts_value_val = nan_value_counts_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "nan-value-counts", nan_value_counts_value_val);
	}

	// Serialize: lower-bounds
	if (lower_bounds.has_value()) {
		auto &lower_bounds_value = *lower_bounds;
		yyjson_mut_val *lower_bounds_value_val = lower_bounds_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "lower-bounds", lower_bounds_value_val);
	}

	// Serialize: upper-bounds
	if (upper_bounds.has_value()) {
		auto &upper_bounds_value = *upper_bounds;
		yyjson_mut_val *upper_bounds_value_val = upper_bounds_value.ToJSON(doc);
		yyjson_mut_obj_add_val(doc, obj, "upper-bounds", upper_bounds_value_val);
	}
}

yyjson_mut_val *DataFile::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
