
#include "rest_catalog/objects/table_identifier.hpp"

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

TableIdentifier::TableIdentifier(Namespace _namespace_p, string name_p)
    : _namespace(std::move(_namespace_p)), name(std::move(name_p)) {
}

TableIdentifierBuilder::TableIdentifierBuilder() {
}

TableIdentifierBuilder &TableIdentifierBuilder::SetNamespace(Namespace value) {
	_namespace_.emplace(std::move(value));
	has__namespace_ = true;
	return *this;
}

TableIdentifierBuilder &TableIdentifierBuilder::SetName(string value) {
	name_.emplace(std::move(value));
	has_name_ = true;
	return *this;
}

TableIdentifier TableIdentifierBuilder::Build() {
	if (!has__namespace_) {
		throw InvalidInputException("TableIdentifier required property 'namespace' is missing");
	}
	if (!has_name_) {
		throw InvalidInputException("TableIdentifier required property 'name' is missing");
	}
	auto result = TableIdentifier(std::move(*_namespace_), std::move(*name_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TableIdentifierBuilder::TryBuild(optional<TableIdentifier> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string TableIdentifier::TryFromJSON(yyjson_val *obj, TableIdentifierBuilder &builder) {
	try {
		auto _namespace_val = yyjson_obj_get(obj, "namespace");
		if (!_namespace_val) {
			throw InvalidInputException("TableIdentifier required property 'namespace' is missing");
		} else {
			builder.SetNamespace(Namespace::FromJSON(_namespace_val));
		}
		auto name_val = yyjson_obj_get(obj, "name");
		if (!name_val) {
			throw InvalidInputException("TableIdentifier required property 'name' is missing");
		} else {
			string name;
			if (yyjson_is_null(name_val)) {
				throw InvalidInputException("TableIdentifier property 'name' is not nullable, but is 'null'");
			} else if (yyjson_is_str(name_val)) {
				name = yyjson_get_str(name_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("TableIdentifier property 'name' is not of type 'string', found '%s' instead",
				                       yyjson_get_type_desc(name_val)));
			}
			builder.SetName(std::move(name));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableIdentifier TableIdentifier::FromJSON(yyjson_val *obj) {
	TableIdentifierBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

TableIdentifier TableIdentifier::Copy() const {
	TableIdentifierBuilder builder;
	auto _namespace_tmp = _namespace.Copy();
	builder.SetNamespace(std::move(_namespace_tmp));
	string name_tmp;
	name_tmp = name;
	builder.SetName(std::move(name_tmp));
	return builder.Build();
}

string TableIdentifier::Validate() const {
	string error;
	error = _namespace.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void TableIdentifier::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: namespace
	yyjson_mut_val *_namespace_val = _namespace.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "namespace", _namespace_val);

	// Serialize: name
	yyjson_mut_obj_add_strcpy(doc, obj, "name", name.c_str());
}

yyjson_mut_val *TableIdentifier::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
