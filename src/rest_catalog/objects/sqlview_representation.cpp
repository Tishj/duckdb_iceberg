
#include "rest_catalog/objects/sqlview_representation.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

SQLViewRepresentation::SQLViewRepresentation() {
}

SQLViewRepresentationBuilder::SQLViewRepresentationBuilder() {
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetType(string value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetSql(string value) {
	result_.sql = std::move(value);
	has_sql_ = true;
	return *this;
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetDialect(string value) {
	result_.dialect = std::move(value);
	has_dialect_ = true;
	return *this;
}

string SQLViewRepresentationBuilder::TryBuild(SQLViewRepresentation &result) {
	if (!has_type_) {
		return "SQLViewRepresentation required property 'type' is missing";
	}
	if (!has_sql_) {
		return "SQLViewRepresentation required property 'sql' is missing";
	}
	if (!has_dialect_) {
		return "SQLViewRepresentation required property 'dialect' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

SQLViewRepresentation SQLViewRepresentationBuilder::Build() {
	SQLViewRepresentation result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

SQLViewRepresentation SQLViewRepresentation::FromJSON(yyjson_val *obj) {
	SQLViewRepresentation res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

SQLViewRepresentation SQLViewRepresentation::Copy() const {
	SQLViewRepresentation res;
	res.type = type;
	res.sql = sql;
	res.dialect = dialect;
	return res;
}

string SQLViewRepresentation::Validate() const {
	string error;
	return "";
}

string SQLViewRepresentation::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "SQLViewRepresentation required property 'type' is missing";
	} else {
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			return StringUtil::Format(
			    "SQLViewRepresentation property 'type' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(type_val));
		}
	}
	auto sql_val = yyjson_obj_get(obj, "sql");
	if (!sql_val) {
		return "SQLViewRepresentation required property 'sql' is missing";
	} else {
		if (yyjson_is_str(sql_val)) {
			sql = yyjson_get_str(sql_val);
		} else {
			return StringUtil::Format(
			    "SQLViewRepresentation property 'sql' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(sql_val));
		}
	}
	auto dialect_val = yyjson_obj_get(obj, "dialect");
	if (!dialect_val) {
		return "SQLViewRepresentation required property 'dialect' is missing";
	} else {
		if (yyjson_is_str(dialect_val)) {
			dialect = yyjson_get_str(dialect_val);
		} else {
			return StringUtil::Format(
			    "SQLViewRepresentation property 'dialect' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(dialect_val));
		}
	}
	return Validate();
}

void SQLViewRepresentation::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: sql
	yyjson_mut_obj_add_strcpy(doc, obj, "sql", sql.c_str());

	// Serialize: dialect
	yyjson_mut_obj_add_strcpy(doc, obj, "dialect", dialect.c_str());
}

yyjson_mut_val *SQLViewRepresentation::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
