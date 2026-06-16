
#include "rest_catalog/objects/assert_default_sort_order_id.hpp"

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

AssertDefaultSortOrderId::AssertDefaultSortOrderId(TableRequirementType type_p, int32_t default_sort_order_id_p)
    : type(std::move(type_p)), default_sort_order_id(std::move(default_sort_order_id_p)) {
}
AssertDefaultSortOrderId::AssertDefaultSortOrderId(const AssertDefaultSortOrderId &other)
    : type(other.type.Copy()), default_sort_order_id(other.default_sort_order_id) {
}
AssertDefaultSortOrderId::AssertDefaultSortOrderId(AssertDefaultSortOrderId &&other)
    : AssertDefaultSortOrderId(static_cast<const AssertDefaultSortOrderId &>(other)) {
}

AssertDefaultSortOrderIdBuilder::AssertDefaultSortOrderIdBuilder() {
}

AssertDefaultSortOrderIdBuilder &AssertDefaultSortOrderIdBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertDefaultSortOrderIdBuilder &AssertDefaultSortOrderIdBuilder::SetDefaultSortOrderId(int32_t value) {
	default_sort_order_id_.emplace(std::move(value));
	has_default_sort_order_id_ = true;
	return *this;
}

AssertDefaultSortOrderId AssertDefaultSortOrderIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertDefaultSortOrderId required property 'type' is missing");
	}
	if (!has_default_sort_order_id_) {
		throw InvalidInputException("AssertDefaultSortOrderId required property 'default-sort-order-id' is missing");
	}
	auto result = AssertDefaultSortOrderId(std::move(*type_), std::move(*default_sort_order_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertDefaultSortOrderIdBuilder::TryBuild(optional<AssertDefaultSortOrderId> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssertDefaultSortOrderId::TryFromJSON(yyjson_val *obj, AssertDefaultSortOrderIdBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertDefaultSortOrderId required property 'type' is missing");
		} else {
			builder.SetType(TableRequirementType::FromJSON(type_val));
		}
		auto default_sort_order_id_val = yyjson_obj_get(obj, "default-sort-order-id");
		if (!default_sort_order_id_val) {
			throw InvalidInputException(
			    "AssertDefaultSortOrderId required property 'default-sort-order-id' is missing");
		} else {
			int32_t default_sort_order_id;
			if (yyjson_is_int(default_sort_order_id_val)) {
				default_sort_order_id = yyjson_get_int(default_sort_order_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertDefaultSortOrderId property 'default_sort_order_id' is not of type "
				                       "'integer', found '%s' instead",
				                       yyjson_get_type_desc(default_sort_order_id_val)));
			}
			builder.SetDefaultSortOrderId(std::move(default_sort_order_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertDefaultSortOrderId AssertDefaultSortOrderId::FromJSON(yyjson_val *obj) {
	AssertDefaultSortOrderIdBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssertDefaultSortOrderId AssertDefaultSortOrderId::Copy() const {
	return AssertDefaultSortOrderId(*this);
}

string AssertDefaultSortOrderId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-default-sort-order-id")) {
		return StringUtil::Format(
		    "AssertDefaultSortOrderId property 'type' must be assert-default-sort-order-id, not %s", type.value);
	}
	return "";
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
