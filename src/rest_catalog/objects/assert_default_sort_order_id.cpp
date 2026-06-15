
#include "rest_catalog/objects/assert_default_sort_order_id.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssertDefaultSortOrderId::AssertDefaultSortOrderId() {
}

AssertDefaultSortOrderIdBuilder::AssertDefaultSortOrderIdBuilder() {
}

AssertDefaultSortOrderIdBuilder &AssertDefaultSortOrderIdBuilder::SetType(TableRequirementType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AssertDefaultSortOrderIdBuilder &AssertDefaultSortOrderIdBuilder::SetDefaultSortOrderId(int32_t value) {
	result_.default_sort_order_id = std::move(value);
	has_default_sort_order_id_ = true;
	return *this;
}

string AssertDefaultSortOrderIdBuilder::TryBuild(AssertDefaultSortOrderId &result) {
	if (!has_type_) {
		return "AssertDefaultSortOrderId required property 'type' is missing";
	}
	if (!has_default_sort_order_id_) {
		return "AssertDefaultSortOrderId required property 'default-sort-order-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssertDefaultSortOrderId AssertDefaultSortOrderIdBuilder::Build() {
	AssertDefaultSortOrderId result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssertDefaultSortOrderId AssertDefaultSortOrderId::FromJSON(yyjson_val *obj) {
	AssertDefaultSortOrderId res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssertDefaultSortOrderId AssertDefaultSortOrderId::Copy() const {
	AssertDefaultSortOrderId res;
	res.type = type.Copy();
	res.default_sort_order_id = default_sort_order_id;
	return res;
}

string AssertDefaultSortOrderId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (type.value != "assert-default-sort-order-id") {
		return "AssertDefaultSortOrderId property 'type' must be assert-default-sort-order-id";
	}
	return "";
}

string AssertDefaultSortOrderId::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AssertDefaultSortOrderId required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto default_sort_order_id_val = yyjson_obj_get(obj, "default-sort-order-id");
	if (!default_sort_order_id_val) {
		return "AssertDefaultSortOrderId required property 'default-sort-order-id' is missing";
	} else {
		if (yyjson_is_int(default_sort_order_id_val)) {
			default_sort_order_id = yyjson_get_int(default_sort_order_id_val);
		} else {
			return StringUtil::Format("AssertDefaultSortOrderId property 'default_sort_order_id' is not of type "
			                          "'integer', found '%s' instead",
			                          yyjson_get_type_desc(default_sort_order_id_val));
		}
	}
	return Validate();
}

void AssertDefaultSortOrderId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: default-sort-order-id
	yyjson_mut_obj_add_int(doc, obj, "default-sort-order-id", default_sort_order_id);
}

yyjson_mut_val *AssertDefaultSortOrderId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
