
#include "rest_catalog/objects/type.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Type::Type() {
}

TypeBuilder::TypeBuilder() {
}

TypeBuilder &TypeBuilder::SetPrimitiveType(PrimitiveType value) {
	result_.primitive_type = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetStructType(StructType value) {
	result_.struct_type = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetListType(ListType value) {
	result_.list_type = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetMapType(MapType value) {
	result_.map_type = std::move(value);
	return *this;
}

string TypeBuilder::TryBuild(Type &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Type TypeBuilder::Build() {
	Type result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Type Type::FromJSON(yyjson_val *obj) {
	Type res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Type Type::Copy() const {
	Type res;
	if (primitive_type.has_value()) {
		res.primitive_type.emplace();
		(*res.primitive_type) = (*primitive_type).Copy();
	}
	if (struct_type.has_value()) {
		res.struct_type.emplace();
		(*res.struct_type) = (*struct_type).Copy();
	}
	if (list_type.has_value()) {
		res.list_type.emplace();
		(*res.list_type) = (*list_type).Copy();
	}
	if (map_type.has_value()) {
		res.map_type.emplace();
		(*res.map_type) = (*map_type).Copy();
	}
	return res;
}

string Type::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (primitive_type.has_value()) {
		matched_one_of_variants++;
		error = primitive_type->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (struct_type.has_value()) {
		matched_one_of_variants++;
		error = struct_type->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (list_type.has_value()) {
		matched_one_of_variants++;
		error = list_type->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (map_type.has_value()) {
		matched_one_of_variants++;
		error = map_type->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "Type must have exactly one oneOf variant set";
	}
	return "";
}

string Type::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		primitive_type.emplace();
		error = primitive_type->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			primitive_type = nullopt;
		}
		struct_type.emplace();
		error = struct_type->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			struct_type = nullopt;
		}
		list_type.emplace();
		error = list_type->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			list_type = nullopt;
		}
		map_type.emplace();
		error = map_type->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			map_type = nullopt;
		}
		return "Type failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
}

yyjson_mut_val *Type::ToJSON(yyjson_mut_doc *doc) const {
	if (primitive_type.has_value()) {
		return primitive_type->ToJSON(doc);
	} else if (struct_type.has_value()) {
		return struct_type->ToJSON(doc);
	} else if (list_type.has_value()) {
		return list_type->ToJSON(doc);
	} else if (map_type.has_value()) {
		return map_type->ToJSON(doc);
	}
	// No variant is active - return empty object
	return yyjson_mut_obj(doc);
}

} // namespace rest_api_objects
} // namespace duckdb
