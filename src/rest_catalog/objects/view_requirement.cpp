
#include "rest_catalog/objects/view_requirement.hpp"

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

ViewRequirement::ViewRequirement(optional<AssertViewUUID> assert_view_uuid_p)
    : assert_view_uuid(std::move(assert_view_uuid_p)) {
}

ViewRequirementBuilder::ViewRequirementBuilder() {
}

ViewRequirementBuilder &ViewRequirementBuilder::SetAssertViewUuid(AssertViewUUID value) {
	assert_view_uuid_.emplace(std::move(value));
	return *this;
}

ViewRequirement ViewRequirementBuilder::Build() {
	auto result = ViewRequirement(std::move(assert_view_uuid_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string ViewRequirementBuilder::TryBuild(optional<ViewRequirement> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string ViewRequirement::TryFromJSON(yyjson_val *obj, ViewRequirementBuilder &builder) {
	try {
		do {
			try {
				builder.SetAssertViewUuid(AssertViewUUID::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			throw InvalidInputException("ViewRequirement failed to parse, none of the oneOf candidates matched");
		} while (false);
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

ViewRequirement ViewRequirement::FromJSON(yyjson_val *obj) {
	ViewRequirementBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

ViewRequirement ViewRequirement::Copy() const {
	ViewRequirementBuilder builder;
	optional<AssertViewUUID> assert_view_uuid_tmp;
	if (assert_view_uuid.has_value()) {
		assert_view_uuid_tmp.emplace((*assert_view_uuid).Copy());
	}
	if (assert_view_uuid_tmp.has_value()) {
		builder.SetAssertViewUuid(std::move(*assert_view_uuid_tmp));
	}
	return builder.Build();
}

string ViewRequirement::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (assert_view_uuid.has_value()) {
		matched_one_of_variants++;
		error = assert_view_uuid->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "ViewRequirement must have exactly one oneOf variant set";
	}
	return "";
}

void ViewRequirement::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (assert_view_uuid.has_value()) {
		assert_view_uuid->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *ViewRequirement::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
