
#include "rest_catalog/objects/value_map.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ValueMap::ValueMap() {
}

ValueMapBuilder::ValueMapBuilder() {
}

ValueMapBuilder &ValueMapBuilder::SetKeys(vector<IntegerTypeValue> value) {
	result_.keys = std::move(value);
	return *this;
}

ValueMapBuilder &ValueMapBuilder::SetValues(vector<PrimitiveTypeValue> value) {
	result_.values = std::move(value);
	return *this;
}

string ValueMapBuilder::TryBuild(ValueMap &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

ValueMap ValueMapBuilder::Build() {
	ValueMap result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

ValueMap ValueMap::FromJSON(yyjson_val *obj) {
	ValueMap res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

ValueMap ValueMap::Copy() const {
	ValueMap res;
	if (keys.has_value()) {
		res.keys.emplace();
		(*res.keys).reserve((*keys).size());
		for (auto &item : (*keys)) {
			(*res.keys).emplace_back(item.Copy());
		}
	}
	if (values.has_value()) {
		res.values.emplace();
		(*res.values).reserve((*values).size());
		for (auto &item : (*values)) {
			(*res.values).emplace_back(item.Copy());
		}
	}
	return res;
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

string ValueMap::TryFromJSON(yyjson_val *obj) {
	string error;
	auto keys_val = yyjson_obj_get(obj, "keys");
	if (keys_val) {
		vector<IntegerTypeValue> keys_tmp;
		if (yyjson_is_arr(keys_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(keys_val, idx, max, val) {
				IntegerTypeValue tmp;
				error = tmp.TryFromJSON(val);
				if (!error.empty()) {
					return error;
				}
				keys_tmp.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("ValueMap property 'keys_tmp' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(keys_val));
		}
		keys = std::move(keys_tmp);
	}
	auto values_val = yyjson_obj_get(obj, "values");
	if (values_val) {
		vector<PrimitiveTypeValue> values_tmp;
		if (yyjson_is_arr(values_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(values_val, idx, max, val) {
				auto tmp = GeneratedObjectAccess::Create<PrimitiveTypeValue>();
				error = tmp.TryFromJSON(val);
				if (!error.empty()) {
					return error;
				}
				values_tmp.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format("ValueMap property 'values_tmp' is not of type 'array', found '%s' instead",
			                          yyjson_get_type_desc(values_val));
		}
		values = std::move(values_tmp);
	}
	return Validate();
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
