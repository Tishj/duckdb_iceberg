
#include "rest_catalog/objects/assert_create.hpp"

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

AssertCreate::AssertCreate(TableRequirementType type_p) : type(std::move(type_p)) {
}

AssertCreateBuilder::AssertCreateBuilder() {
}

AssertCreateBuilder &AssertCreateBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertCreate AssertCreateBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertCreate required property 'type' is missing");
	}
	auto result = AssertCreate(std::move(*type_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertCreateBuilder::TryBuild(optional<AssertCreate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertCreate AssertCreate::FromJSON(yyjson_val *obj) {
	AssertCreateBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("AssertCreate required property 'type' is missing");
	} else {
		builder.SetType(TableRequirementType::FromJSON(type_val));
	}
	return builder.Build();
}

string AssertCreate::TryFromJSON(yyjson_val *obj, optional<AssertCreate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertCreate AssertCreate::Copy() const {
	AssertCreateBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	return builder.Build();
}

string AssertCreate::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-create")) {
		return StringUtil::Format("AssertCreate property 'type' must be assert-create, not %s", type.value);
	}
	return "";
}

void AssertCreate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);
}

yyjson_mut_val *AssertCreate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
