
#include "rest_catalog/objects/sort_field.hpp"

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

SortField::SortField(int32_t source_id_p, Transform transform_p, SortDirection direction_p, NullOrder null_order_p)
    : source_id(std::move(source_id_p)), transform(std::move(transform_p)), direction(std::move(direction_p)),
      null_order(std::move(null_order_p)) {
}
SortField::SortField(const SortField &other)
    : source_id(other.source_id), transform(other.transform.Copy()), direction(other.direction.Copy()),
      null_order(other.null_order.Copy()) {
}
SortField::SortField(SortField &&other) : SortField(static_cast<const SortField &>(other)) {
}

SortFieldBuilder::SortFieldBuilder() {
}

SortFieldBuilder &SortFieldBuilder::SetSourceId(int32_t value) {
	source_id_.emplace(std::move(value));
	has_source_id_ = true;
	return *this;
}

SortFieldBuilder &SortFieldBuilder::SetTransform(Transform value) {
	transform_.emplace(std::move(value));
	has_transform_ = true;
	return *this;
}

SortFieldBuilder &SortFieldBuilder::SetDirection(SortDirection value) {
	direction_.emplace(std::move(value));
	has_direction_ = true;
	return *this;
}

SortFieldBuilder &SortFieldBuilder::SetNullOrder(NullOrder value) {
	null_order_.emplace(std::move(value));
	has_null_order_ = true;
	return *this;
}

SortField SortFieldBuilder::Build() {
	if (!has_source_id_) {
		throw InvalidInputException("SortField required property 'source-id' is missing");
	}
	if (!has_transform_) {
		throw InvalidInputException("SortField required property 'transform' is missing");
	}
	if (!has_direction_) {
		throw InvalidInputException("SortField required property 'direction' is missing");
	}
	if (!has_null_order_) {
		throw InvalidInputException("SortField required property 'null-order' is missing");
	}
	auto result =
	    SortField(std::move(*source_id_), std::move(*transform_), std::move(*direction_), std::move(*null_order_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SortFieldBuilder::TryBuild(optional<SortField> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string SortField::TryFromJSON(yyjson_val *obj, SortFieldBuilder &builder) {
	try {
		auto source_id_val = yyjson_obj_get(obj, "source-id");
		if (!source_id_val) {
			throw InvalidInputException("SortField required property 'source-id' is missing");
		} else {
			int32_t source_id;
			if (yyjson_is_int(source_id_val)) {
				source_id = yyjson_get_int(source_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("SortField property 'source_id' is not of type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(source_id_val)));
			}
			builder.SetSourceId(std::move(source_id));
		}
		auto transform_val = yyjson_obj_get(obj, "transform");
		if (!transform_val) {
			throw InvalidInputException("SortField required property 'transform' is missing");
		} else {
			builder.SetTransform(Transform::FromJSON(transform_val));
		}
		auto direction_val = yyjson_obj_get(obj, "direction");
		if (!direction_val) {
			throw InvalidInputException("SortField required property 'direction' is missing");
		} else {
			builder.SetDirection(SortDirection::FromJSON(direction_val));
		}
		auto null_order_val = yyjson_obj_get(obj, "null-order");
		if (!null_order_val) {
			throw InvalidInputException("SortField required property 'null-order' is missing");
		} else {
			builder.SetNullOrder(NullOrder::FromJSON(null_order_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SortField SortField::FromJSON(yyjson_val *obj) {
	SortFieldBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

SortField SortField::Copy() const {
	return SortField(*this);
}

string SortField::Validate() const {
	string error;
	error = transform.Validate();
	if (!error.empty()) {
		return error;
	}
	error = direction.Validate();
	if (!error.empty()) {
		return error;
	}
	error = null_order.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void SortField::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: source-id
	yyjson_mut_obj_add_int(doc, obj, "source-id", source_id);

	// Serialize: transform
	yyjson_mut_val *transform_val = transform.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "transform", transform_val);

	// Serialize: direction
	yyjson_mut_val *direction_val = direction.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "direction", direction_val);

	// Serialize: null-order
	yyjson_mut_val *null_order_val = null_order.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "null-order", null_order_val);
}

yyjson_mut_val *SortField::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
