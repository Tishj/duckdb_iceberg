
#include "rest_catalog/objects/view_representation.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

ViewRepresentation::ViewRepresentation(optional<SQLViewRepresentation> sqlview_representation_p)
    : sqlview_representation(std::move(sqlview_representation_p)) {
}

ViewRepresentationBuilder::ViewRepresentationBuilder() {
}

ViewRepresentationBuilder &ViewRepresentationBuilder::SetSqlviewRepresentation(SQLViewRepresentation value) {
	sqlview_representation_ = std::move(value);
	return *this;
}

ViewRepresentation ViewRepresentationBuilder::Build() {
	auto result = ViewRepresentation(std::move(sqlview_representation_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewRepresentationBuilder::TryBuild(optional<ViewRepresentation> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewRepresentation ViewRepresentation::FromJSON(yyjson_val *obj) {
	ViewRepresentationBuilder builder;
	do {
		try {
			builder.SetSqlviewRepresentation(SQLViewRepresentation::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		throw InvalidInputException("ViewRepresentation failed to parse, none of the oneOf candidates matched");
	} while (false);
	return builder.Build();
}

string ViewRepresentation::TryFromJSON(yyjson_val *obj, optional<ViewRepresentation> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewRepresentation ViewRepresentation::Copy() const {
	ViewRepresentationBuilder builder;
	optional<SQLViewRepresentation> sqlview_representation_tmp;
	if (sqlview_representation.has_value()) {
		sqlview_representation_tmp.emplace();
		(*sqlview_representation_tmp) = (*sqlview_representation).Copy();
	}
	if (sqlview_representation_tmp.has_value()) {
		builder.SetSqlviewRepresentation(std::move(*sqlview_representation_tmp));
	}
	return builder.Build();
}

string ViewRepresentation::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (sqlview_representation.has_value()) {
		matched_one_of_variants++;
		error = sqlview_representation->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "ViewRepresentation must have exactly one oneOf variant set";
	}
	return "";
}

void ViewRepresentation::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (sqlview_representation.has_value()) {
		sqlview_representation->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *ViewRepresentation::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
