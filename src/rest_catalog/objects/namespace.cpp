
#include "rest_catalog/objects/namespace.hpp"

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

Namespace::Namespace(vector<string> value_p) : value(std::move(value_p)) {
}
Namespace::Namespace(const Namespace &other)
    : value(([&]() {
	      vector<string> copied;
	      copied.reserve(other.value.size());
	      for (const auto &item : other.value) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())) {
}
Namespace::Namespace(Namespace &&other) : Namespace(static_cast<const Namespace &>(other)) {
}

string Namespace::TryFromJSON(yyjson_val *obj, optional<Namespace> &result) {
	try {
		vector<string> value;
		if (yyjson_is_arr(obj)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(obj, idx, max, val) {
				string tmp;
				if (yyjson_is_str(val)) {
					tmp = yyjson_get_str(val);
				} else {
					throw InvalidInputException(
					    StringUtil::Format("Namespace property 'tmp' is not of type 'string', found '%s' instead",
					                       yyjson_get_type_desc(val)));
				}
				value.emplace_back(std::move(tmp));
			}
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "Namespace property 'value' is not of type 'array', found '%s' instead", yyjson_get_type_desc(obj)));
		}
		result.emplace(Namespace(std::move(value)));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Namespace Namespace::FromJSON(yyjson_val *obj) {
	optional<Namespace> result;
	auto error = TryFromJSON(obj, result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	if (!result.has_value()) {
		throw InternalException("TryFromJSON succeeded without producing a result");
	}
	return std::move(*result);
}

Namespace Namespace::Copy() const {
	return Namespace(*this);
}

string Namespace::Validate() const {
	string error;
	return "";
}

yyjson_mut_val *Namespace::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *arr = yyjson_mut_arr(doc);
	for (const auto &item : value) {
		yyjson_mut_arr_append(arr, yyjson_mut_str(doc, item.c_str()));
	}
	return arr;
}

} // namespace rest_api_objects
} // namespace duckdb
