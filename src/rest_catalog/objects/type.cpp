
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

Type::Type(optional<PrimitiveType> primitive_type_p, optional<StructType> struct_type_p, optional<ListType> list_type_p,
           optional<MapType> map_type_p)
    : primitive_type(std::move(primitive_type_p)), struct_type(std::move(struct_type_p)),
      list_type(std::move(list_type_p)), map_type(std::move(map_type_p)) {
}

TypeBuilder::TypeBuilder() {
}

TypeBuilder &TypeBuilder::SetPrimitiveType(PrimitiveType value) {
	primitive_type_ = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetStructType(StructType value) {
	struct_type_ = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetListType(ListType value) {
	list_type_ = std::move(value);
	return *this;
}

TypeBuilder &TypeBuilder::SetMapType(MapType value) {
	map_type_ = std::move(value);
	return *this;
}

Type TypeBuilder::Build() {
	auto result =
	    Type(std::move(primitive_type_), std::move(struct_type_), std::move(list_type_), std::move(map_type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TypeBuilder::TryBuild(optional<Type> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Type Type::FromJSON(yyjson_val *obj) {
	TypeBuilder builder;
	do {
		try {
			builder.SetPrimitiveType(PrimitiveType::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetStructType(StructType::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetListType(ListType::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetMapType(MapType::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		throw InvalidInputException("Type failed to parse, none of the oneOf candidates matched");
	} while (false);
	return builder.Build();
}

string Type::TryFromJSON(yyjson_val *obj, optional<Type> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Type Type::Copy() const {
	TypeBuilder builder;
	optional<PrimitiveType> primitive_type_tmp;
	if (primitive_type.has_value()) {
		primitive_type_tmp.emplace();
		(*primitive_type_tmp) = (*primitive_type).Copy();
	}
	if (primitive_type_tmp.has_value()) {
		builder.SetPrimitiveType(std::move(*primitive_type_tmp));
	}
	optional<StructType> struct_type_tmp;
	if (struct_type.has_value()) {
		struct_type_tmp.emplace();
		(*struct_type_tmp) = (*struct_type).Copy();
	}
	if (struct_type_tmp.has_value()) {
		builder.SetStructType(std::move(*struct_type_tmp));
	}
	optional<ListType> list_type_tmp;
	if (list_type.has_value()) {
		list_type_tmp.emplace();
		(*list_type_tmp) = (*list_type).Copy();
	}
	if (list_type_tmp.has_value()) {
		builder.SetListType(std::move(*list_type_tmp));
	}
	optional<MapType> map_type_tmp;
	if (map_type.has_value()) {
		map_type_tmp.emplace();
		(*map_type_tmp) = (*map_type).Copy();
	}
	if (map_type_tmp.has_value()) {
		builder.SetMapType(std::move(*map_type_tmp));
	}
	return builder.Build();
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
