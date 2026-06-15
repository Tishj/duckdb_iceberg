
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

SQLViewRepresentation::SQLViewRepresentation(string type_p, string sql_p, string dialect_p)
    : type(std::move(type_p)), sql(std::move(sql_p)), dialect(std::move(dialect_p)) {
}

SQLViewRepresentationBuilder::SQLViewRepresentationBuilder() {
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetType(string value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetSql(string value) {
	sql_ = std::move(value);
	has_sql_ = true;
	return *this;
}

SQLViewRepresentationBuilder &SQLViewRepresentationBuilder::SetDialect(string value) {
	dialect_ = std::move(value);
	has_dialect_ = true;
	return *this;
}

SQLViewRepresentation SQLViewRepresentationBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("SQLViewRepresentation required property 'type' is missing");
	}
	if (!has_sql_) {
		throw InvalidInputException("SQLViewRepresentation required property 'sql' is missing");
	}
	if (!has_dialect_) {
		throw InvalidInputException("SQLViewRepresentation required property 'dialect' is missing");
	}
	auto result = SQLViewRepresentation(std::move(*type_), std::move(*sql_), std::move(*dialect_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string SQLViewRepresentationBuilder::TryBuild(optional<SQLViewRepresentation> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SQLViewRepresentation SQLViewRepresentation::FromJSON(yyjson_val *obj) {
	SQLViewRepresentationBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("SQLViewRepresentation required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SQLViewRepresentation property 'type' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto sql_val = yyjson_obj_get(obj, "sql");
	if (!sql_val) {
		throw InvalidInputException("SQLViewRepresentation required property 'sql' is missing");
	} else {
		string sql;
		if (yyjson_is_str(sql_val)) {
			sql = yyjson_get_str(sql_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("SQLViewRepresentation property 'sql' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(sql_val)));
		}
		builder.SetSql(std::move(sql));
	}
	auto dialect_val = yyjson_obj_get(obj, "dialect");
	if (!dialect_val) {
		throw InvalidInputException("SQLViewRepresentation required property 'dialect' is missing");
	} else {
		string dialect;
		if (yyjson_is_str(dialect_val)) {
			dialect = yyjson_get_str(dialect_val);
		} else {
			throw InvalidInputException(StringUtil::Format(
			    "SQLViewRepresentation property 'dialect' is not of type 'string', found '%s' instead",
			    yyjson_get_type_desc(dialect_val)));
		}
		builder.SetDialect(std::move(dialect));
	}
	return builder.Build();
}

string SQLViewRepresentation::TryFromJSON(yyjson_val *obj, optional<SQLViewRepresentation> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SQLViewRepresentation SQLViewRepresentation::Copy() const {
	SQLViewRepresentationBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	string sql_tmp;
	sql_tmp = sql;
	builder.SetSql(std::move(sql_tmp));
	string dialect_tmp;
	dialect_tmp = dialect;
	builder.SetDialect(std::move(dialect_tmp));
	return builder.Build();
}

string SQLViewRepresentation::Validate() const {
	string error;
	return "";
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
