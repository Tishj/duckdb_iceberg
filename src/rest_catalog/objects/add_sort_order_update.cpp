
#include "rest_catalog/objects/add_sort_order_update.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AddSortOrderUpdate::AddSortOrderUpdate(BaseUpdate base_update_p, SortOrder sort_order_p)
    : base_update(std::move(base_update_p)), sort_order(std::move(sort_order_p)) {
}

AddSortOrderUpdateBuilder::AddSortOrderUpdateBuilder() {
}

AddSortOrderUpdateBuilder &AddSortOrderUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_ = std::move(value);
	return *this;
}

AddSortOrderUpdateBuilder &AddSortOrderUpdateBuilder::SetSortOrder(SortOrder value) {
	sort_order_ = std::move(value);
	has_sort_order_ = true;
	return *this;
}

AddSortOrderUpdate AddSortOrderUpdateBuilder::Build() {
	if (!has_sort_order_) {
		throw InvalidInputException("AddSortOrderUpdate required property 'sort-order' is missing");
	}
	auto result = AddSortOrderUpdate(std::move(*base_update_), std::move(*sort_order_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AddSortOrderUpdateBuilder::TryBuild(optional<AddSortOrderUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddSortOrderUpdate AddSortOrderUpdate::FromJSON(yyjson_val *obj) {
	AddSortOrderUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto sort_order_val = yyjson_obj_get(obj, "sort-order");
	if (!sort_order_val) {
		throw InvalidInputException("AddSortOrderUpdate required property 'sort-order' is missing");
	} else {
		optional<SortOrder> sort_order;
		sort_order = SortOrder::FromJSON(sort_order_val);
		builder.SetSortOrder(std::move(*sort_order));
	}
	return builder.Build();
}

string AddSortOrderUpdate::TryFromJSON(yyjson_val *obj, optional<AddSortOrderUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddSortOrderUpdate AddSortOrderUpdate::Copy() const {
	AddSortOrderUpdateBuilder builder;
	optional<BaseUpdate> base_update_tmp;
	base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(*base_update_tmp));
	optional<SortOrder> sort_order_tmp;
	sort_order_tmp = sort_order.Copy();
	builder.SetSortOrder(std::move(*sort_order_tmp));
	return builder.Build();
}

string AddSortOrderUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = sort_order.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void AddSortOrderUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: sort-order
	yyjson_mut_val *sort_order_val = sort_order.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "sort-order", sort_order_val);
}

yyjson_mut_val *AddSortOrderUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
