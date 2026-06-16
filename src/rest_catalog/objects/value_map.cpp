
#include "rest_catalog/objects/value_map.hpp"

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

ValueMap::ValueMap(optional<vector<IntegerTypeValue>> keys_p, optional<vector<PrimitiveTypeValue>> values_p)
    : keys(std::move(keys_p)), values(std::move(values_p)) {
}
ValueMap::ValueMap(const ValueMap &other)
    : keys((other.keys.has_value() ? optional<vector<IntegerTypeValue>>(([&]() {
	      vector<IntegerTypeValue> copied;
	      copied.reserve((*other.keys).size());
	      for (const auto &item : (*other.keys)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                   : optional<vector<IntegerTypeValue>>())),
      values((other.values.has_value() ? optional<vector<PrimitiveTypeValue>>(([&]() {
	      vector<PrimitiveTypeValue> copied;
	      copied.reserve((*other.values).size());
	      for (const auto &item : (*other.values)) {
		      copied.emplace_back(item.Copy());
	      }
	      return copied;
      }()))
                                       : optional<vector<PrimitiveTypeValue>>())) {
}
ValueMap::ValueMap(ValueMap &&other) : ValueMap(static_cast<const ValueMap &>(other)) {
}

ValueMapBuilder::ValueMapBuilder() {
}

ValueMapBuilder &ValueMapBuilder::SetKeys(vector<IntegerTypeValue> value) {
	keys_.emplace(std::move(value));
	return *this;
}

ValueMapBuilder &ValueMapBuilder::SetValues(vector<PrimitiveTypeValue> value) {
	values_.emplace(std::move(value));
	return *this;
}

ValueMap ValueMapBuilder::Build() {
	auto result = ValueMap(std::move(keys_), std::move(values_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> ValueMapBuilder::TryBuild(optional<ValueMap> &result) {
	auto built = ValueMap(std::move(keys_), std::move(values_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> ValueMap::TryFromJSON(yyjson_val *obj, ValueMapBuilder &builder) {
	try {
		auto keys_val = yyjson_obj_get(obj, "keys");
		if (keys_val) {
			vector<IntegerTypeValue> keys;
			if (yyjson_is_arr(keys_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(keys_val, idx, max, val) {
					auto tmp = IntegerTypeValue::FromJSON(val);
					keys.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ValueMap property 'keys' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(keys_val)));
			}
			builder.SetKeys(std::move(keys));
		}
		auto values_val = yyjson_obj_get(obj, "values");
		if (values_val) {
			vector<PrimitiveTypeValue> values;
			if (yyjson_is_arr(values_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(values_val, idx, max, val) {
					auto tmp = PrimitiveTypeValue::FromJSON(val);
					values.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(
				    StringUtil::Format("ValueMap property 'values' is not of type 'array', found '%s' instead",
				                       yyjson_get_type_desc(values_val)));
			}
			builder.SetValues(std::move(values));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ValueMap ValueMap::FromJSON(yyjson_val *obj) {
	ValueMapBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

ValueMap ValueMap::Copy() const {
	return ValueMap(*this);
}

optional<string> ValueMap::Validate() const {
	optional<string> error;
	if (keys.has_value()) {
		for (const auto &item : (*keys)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	if (values.has_value()) {
		for (const auto &item : (*values)) {
			error = item.Validate();
			if (error) {
				return error;
			}
		}
	}
	return nullopt;
}

void ValueMap::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: keys
	if (keys.has_value()) {
		auto &keys_value = *keys;
		yyjson_mut_val *keys_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : keys_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(keys_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "keys", keys_value_arr);
	}

	// Serialize: values
	if (values.has_value()) {
		auto &values_value = *values;
		yyjson_mut_val *values_value_arr = yyjson_mut_arr(doc);
		for (const auto &item : values_value) {
			yyjson_mut_val *item_val = item.ToJSON(doc);
			yyjson_mut_arr_append(values_value_arr, item_val);
		}
		yyjson_mut_obj_add_val(doc, obj, "values", values_value_arr);
	}
}

yyjson_mut_val *ValueMap::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
