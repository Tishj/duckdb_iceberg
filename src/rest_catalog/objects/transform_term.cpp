
#include "rest_catalog/objects/transform_term.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

TransformTerm::TransformTerm(string type_p, Transform transform_p, Reference term_p)
    : type(std::move(type_p)), transform(std::move(transform_p)), term(std::move(term_p)) {
}

TransformTermBuilder::TransformTermBuilder() {
}

TransformTermBuilder &TransformTermBuilder::SetType(string value) {
	type_ = std::move(value);
	has_type_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTransform(Transform value) {
	transform_ = std::move(value);
	has_transform_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTerm(Reference value) {
	term_ = std::move(value);
	has_term_ = true;
	return *this;
}

TransformTerm TransformTermBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("TransformTerm required property 'type' is missing");
	}
	if (!has_transform_) {
		throw InvalidInputException("TransformTerm required property 'transform' is missing");
	}
	if (!has_term_) {
		throw InvalidInputException("TransformTerm required property 'term' is missing");
	}
	auto result = TransformTerm(std::move(*type_), std::move(*transform_), std::move(*term_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TransformTermBuilder::TryBuild(optional<TransformTerm> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TransformTerm TransformTerm::FromJSON(yyjson_val *obj) {
	TransformTermBuilder builder;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		throw InvalidInputException("TransformTerm required property 'type' is missing");
	} else {
		string type;
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			throw InvalidInputException(
			    StringUtil::Format("TransformTerm property 'type' is not of type 'string', found '%s' instead",
			                       yyjson_get_type_desc(type_val)));
		}
		builder.SetType(std::move(type));
	}
	auto transform_val = yyjson_obj_get(obj, "transform");
	if (!transform_val) {
		throw InvalidInputException("TransformTerm required property 'transform' is missing");
	} else {
		optional<Transform> transform;
		transform = Transform::FromJSON(transform_val);
		builder.SetTransform(std::move(*transform));
	}
	auto term_val = yyjson_obj_get(obj, "term");
	if (!term_val) {
		throw InvalidInputException("TransformTerm required property 'term' is missing");
	} else {
		optional<Reference> term;
		term = Reference::FromJSON(term_val);
		builder.SetTerm(std::move(*term));
	}
	return builder.Build();
}

string TransformTerm::TryFromJSON(yyjson_val *obj, optional<TransformTerm> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TransformTerm TransformTerm::Copy() const {
	TransformTermBuilder builder;
	string type_tmp;
	type_tmp = type;
	builder.SetType(std::move(type_tmp));
	optional<Transform> transform_tmp;
	transform_tmp = transform.Copy();
	builder.SetTransform(std::move(*transform_tmp));
	optional<Reference> term_tmp;
	term_tmp = term.Copy();
	builder.SetTerm(std::move(*term_tmp));
	return builder.Build();
}

string TransformTerm::Validate() const {
	string error;
	if (!StringUtil::CIEquals(type, "transform")) {
		return StringUtil::Format("TransformTerm property 'type' must be transform, not %s", type);
	}
	error = transform.Validate();
	if (!error.empty()) {
		return error;
	}
	error = term.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void TransformTerm::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_obj_add_strcpy(doc, obj, "type", type.c_str());

	// Serialize: transform
	yyjson_mut_val *transform_val = transform.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "transform", transform_val);

	// Serialize: term
	yyjson_mut_val *term_val = term.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "term", term_val);
}

yyjson_mut_val *TransformTerm::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
