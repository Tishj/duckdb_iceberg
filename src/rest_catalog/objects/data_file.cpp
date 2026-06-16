
#include "rest_catalog/objects/data_file.hpp"

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

DataFile::DataFile(ContentFile content_file_p, optional<int64_t> first_row_id_p, optional<CountMap> column_sizes_p,
                   optional<CountMap> value_counts_p, optional<CountMap> null_value_counts_p,
                   optional<CountMap> nan_value_counts_p, optional<ValueMap> lower_bounds_p,
                   optional<ValueMap> upper_bounds_p)
    : content_file(std::move(content_file_p)), first_row_id(std::move(first_row_id_p)),
      column_sizes(std::move(column_sizes_p)), value_counts(std::move(value_counts_p)),
      null_value_counts(std::move(null_value_counts_p)), nan_value_counts(std::move(nan_value_counts_p)),
      lower_bounds(std::move(lower_bounds_p)), upper_bounds(std::move(upper_bounds_p)) {
}

DataFileBuilder::DataFileBuilder() {
}

DataFileBuilder &DataFileBuilder::SetContentFile(ContentFile value) {
	content_file_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetFirstRowId(int64_t value) {
	first_row_id_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetColumnSizes(CountMap value) {
	column_sizes_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetValueCounts(CountMap value) {
	value_counts_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetNullValueCounts(CountMap value) {
	null_value_counts_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetNanValueCounts(CountMap value) {
	nan_value_counts_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetLowerBounds(ValueMap value) {
	lower_bounds_.emplace(std::move(value));
	return *this;
}

DataFileBuilder &DataFileBuilder::SetUpperBounds(ValueMap value) {
	upper_bounds_.emplace(std::move(value));
	return *this;
}

DataFile DataFileBuilder::Build() {
	auto result = DataFile(std::move(*content_file_), std::move(first_row_id_), std::move(column_sizes_),
	                       std::move(value_counts_), std::move(null_value_counts_), std::move(nan_value_counts_),
	                       std::move(lower_bounds_), std::move(upper_bounds_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string DataFileBuilder::TryBuild(optional<DataFile> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string DataFile::TryFromJSON(yyjson_val *obj, DataFileBuilder &builder) {
	try {
		builder.SetContentFile(ContentFile::FromJSON(obj));
		auto first_row_id_val = yyjson_obj_get(obj, "first-row-id");
		if (first_row_id_val) {
			int64_t first_row_id;
			if (yyjson_is_sint(first_row_id_val)) {
				first_row_id = yyjson_get_sint(first_row_id_val);
			} else if (yyjson_is_uint(first_row_id_val)) {
				first_row_id = yyjson_get_uint(first_row_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("DataFile property 'first_row_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(first_row_id_val)));
			}
			builder.SetFirstRowId(std::move(first_row_id));
		}
		auto column_sizes_val = yyjson_obj_get(obj, "column-sizes");
		if (column_sizes_val) {
			builder.SetColumnSizes(CountMap::FromJSON(column_sizes_val));
		}
		auto value_counts_val = yyjson_obj_get(obj, "value-counts");
		if (value_counts_val) {
			builder.SetValueCounts(CountMap::FromJSON(value_counts_val));
		}
		auto null_value_counts_val = yyjson_obj_get(obj, "null-value-counts");
		if (null_value_counts_val) {
			builder.SetNullValueCounts(CountMap::FromJSON(null_value_counts_val));
		}
		auto nan_value_counts_val = yyjson_obj_get(obj, "nan-value-counts");
		if (nan_value_counts_val) {
			builder.SetNanValueCounts(CountMap::FromJSON(nan_value_counts_val));
		}
		auto lower_bounds_val = yyjson_obj_get(obj, "lower-bounds");
		if (lower_bounds_val) {
			builder.SetLowerBounds(ValueMap::FromJSON(lower_bounds_val));
		}
		auto upper_bounds_val = yyjson_obj_get(obj, "upper-bounds");
		if (upper_bounds_val) {
			builder.SetUpperBounds(ValueMap::FromJSON(upper_bounds_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

DataFile DataFile::FromJSON(yyjson_val *obj) {
	DataFileBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

DataFile DataFile::Copy() const {
	DataFileBuilder builder;
	auto content_file_tmp = content_file.Copy();
	builder.SetContentFile(std::move(content_file_tmp));
	optional<int64_t> first_row_id_tmp;
	if (first_row_id.has_value()) {
		first_row_id_tmp.emplace();
		(*first_row_id_tmp) = (*first_row_id);
	}
	if (first_row_id_tmp.has_value()) {
		builder.SetFirstRowId(std::move((*first_row_id_tmp)));
	}
	optional<CountMap> column_sizes_tmp;
	if (column_sizes.has_value()) {
		column_sizes_tmp.emplace((*column_sizes).Copy());
	}
	if (column_sizes_tmp.has_value()) {
		builder.SetColumnSizes(std::move(*column_sizes_tmp));
	}
	optional<CountMap> value_counts_tmp;
	if (value_counts.has_value()) {
		value_counts_tmp.emplace((*value_counts).Copy());
	}
	if (value_counts_tmp.has_value()) {
		builder.SetValueCounts(std::move(*value_counts_tmp));
	}
	optional<CountMap> null_value_counts_tmp;
	if (null_value_counts.has_value()) {
		null_value_counts_tmp.emplace((*null_value_counts).Copy());
	}
	if (null_value_counts_tmp.has_value()) {
		builder.SetNullValueCounts(std::move(*null_value_counts_tmp));
	}
	optional<CountMap> nan_value_counts_tmp;
	if (nan_value_counts.has_value()) {
		nan_value_counts_tmp.emplace((*nan_value_counts).Copy());
	}
	if (nan_value_counts_tmp.has_value()) {
		builder.SetNanValueCounts(std::move(*nan_value_counts_tmp));
	}
	optional<ValueMap> lower_bounds_tmp;
	if (lower_bounds.has_value()) {
		lower_bounds_tmp.emplace((*lower_bounds).Copy());
	}
	if (lower_bounds_tmp.has_value()) {
		builder.SetLowerBounds(std::move(*lower_bounds_tmp));
	}
	optional<ValueMap> upper_bounds_tmp;
	if (upper_bounds.has_value()) {
		upper_bounds_tmp.emplace((*upper_bounds).Copy());
	}
	if (upper_bounds_tmp.has_value()) {
		builder.SetUpperBounds(std::move(*upper_bounds_tmp));
	}
	return builder.Build();
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
