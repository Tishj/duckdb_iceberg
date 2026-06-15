
#include "rest_catalog/objects/sort_order.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SortOrder::SortOrder(int32_t order_id_p, vector<SortField> fields_p)
    : order_id(std::move(order_id_p)), fields(std::move(fields_p)) {
}

SortOrderBuilder::SortOrderBuilder() {
}

SortOrderBuilder &SortOrderBuilder::SetOrderId(int32_t value) {
	order_id_ = std::move(value);
	has_order_id_ = true;
	return *this;
}

SortOrderBuilder &SortOrderBuilder::SetFields(vector<SortField> value) {
	fields_ = std::move(value);
	has_fields_ = true;
	return *this;
}

SortOrder SortOrderBuilder::Build() {
	if (!has_order_id_) {
		throw InvalidInputException("SortOrder required property 'order-id' is missing");
	}
	if (!has_fields_) {
		throw InvalidInputException("SortOrder required property 'fields' is missing");
	}
	auto result = SortOrder(std::move(*order_id_), std::move(*fields_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SortOrderBuilder::TryBuild(optional<SortOrder> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SortOrder SortOrder::FromJSON(yyjson_val *obj) {
	SortOrderBuilder builder;
	auto order_id_val = yyjson_obj_get(obj, "order-id");
	if (!order_id_val) {
		throw InvalidInputException("SortOrder required property 'order-id' is missing");
	} else {
		int32_t order_id;
		if (yyjson_is_int(order_id_val)) {
			order_id = yyjson_get_int(order_id_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SortOrder property 'order_id' is not of type 'integer', found '%s' instead",
			                       yyjson_get_type_desc(order_id_val)));
		}
		builder.SetOrderId(std::move(order_id));
	}
	auto fields_val = yyjson_obj_get(obj, "fields");
	if (!fields_val) {
		throw InvalidInputException("SortOrder required property 'fields' is missing");
	} else {
		vector<SortField> fields;
		if (yyjson_is_arr(fields_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(fields_val, idx, max, val) {
				auto tmp = SortField::FromJSON(val);
				fields.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("SortOrder property 'fields' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(fields_val));
		}
		builder.SetFields(std::move(fields));
	}
	return builder.Build();
}

string SortOrder::TryFromJSON(yyjson_val *obj, optional<SortOrder> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SortOrder SortOrder::Copy() const {
	SortOrderBuilder builder;
	int32_t order_id_tmp;
	order_id_tmp = order_id;
	builder.SetOrderId(std::move(order_id_tmp));
	vector<SortField> fields_tmp;
	fields_tmp.reserve(fields.size());
	for (auto &item : fields) {
		fields_tmp.emplace_back(item.Copy());
	}
	builder.SetFields(std::move(fields_tmp));
	return builder.Build();
}

string SortOrder::Validate() const {
	string error;
	for (const auto &item : fields) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void SortOrder::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: order-id
	yyjson_mut_obj_add_int(doc, obj, "order-id", order_id);

	// Serialize: fields
	yyjson_mut_val *fields_arr = yyjson_mut_arr(doc);
	for (const auto &item : fields) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(fields_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "fields", fields_arr);
}

yyjson_mut_val *SortOrder::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
