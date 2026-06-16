
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
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ValueMapBuilder::TryBuild(optional<ValueMap> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ValueMap ValueMap::FromJSON(yyjson_val *obj) {
	ValueMapBuilder builder;
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
			throw InvalidInputException(StringUtil::Format(
			    "ValueMap property 'keys' is not of type 'array', found '%s' instead", yyjson_get_type_desc(keys_val)));
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
	return builder.Build();
}

string ValueMap::TryFromJSON(yyjson_val *obj, optional<ValueMap> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ValueMap ValueMap::Copy() const {
	ValueMapBuilder builder;
	optional<vector<IntegerTypeValue>> keys_tmp;
	if (keys.has_value()) {
		keys_tmp.emplace();
		(*keys_tmp).reserve((*keys).size());
		for (auto &item : (*keys)) {
			(*keys_tmp).emplace_back(item.Copy());
		}
	}
	if (keys_tmp.has_value()) {
		builder.SetKeys(std::move((*keys_tmp)));
	}
	optional<vector<PrimitiveTypeValue>> values_tmp;
	if (values.has_value()) {
		values_tmp.emplace();
		(*values_tmp).reserve((*values).size());
		for (auto &item : (*values)) {
			(*values_tmp).emplace_back(item.Copy());
		}
	}
	if (values_tmp.has_value()) {
		builder.SetValues(std::move((*values_tmp)));
	}
	return builder.Build();
}

string ValueMap::Validate() const {
	string error;
	if (keys.has_value()) {
		for (const auto &item : (*keys)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	if (values.has_value()) {
		for (const auto &item : (*values)) {
			error = item.Validate();
			if (!error.empty()) {
				return error;
			}
		}
	}
	return "";
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
