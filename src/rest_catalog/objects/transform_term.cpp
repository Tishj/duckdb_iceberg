
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

TransformTerm::TransformTerm() {
}

TransformTermBuilder::TransformTermBuilder() {
}

TransformTermBuilder &TransformTermBuilder::SetType(string value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTransform(Transform value) {
	result_.transform = std::move(value);
	has_transform_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTerm(Reference value) {
	result_.term = std::move(value);
	has_term_ = true;
	return *this;
}

string TransformTermBuilder::TryBuild(TransformTerm &result) {
	if (!has_type_) {
		return "TransformTerm required property 'type' is missing";
	}
	if (!has_transform_) {
		return "TransformTerm required property 'transform' is missing";
	}
	if (!has_term_) {
		return "TransformTerm required property 'term' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

TransformTerm TransformTermBuilder::Build() {
	TransformTerm result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

TransformTerm TransformTerm::FromJSON(yyjson_val *obj) {
	TransformTerm res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

TransformTerm TransformTerm::Copy() const {
	TransformTerm res;
	res.type = type;
	res.transform = transform.Copy();
	res.term = term.Copy();
	return res;
}

string TransformTerm::Validate() const {
	string error;
	if (type != "transform") {
		return "TransformTerm property 'type' must be transform";
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

string TransformTerm::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "TransformTerm required property 'type' is missing";
	} else {
		if (yyjson_is_str(type_val)) {
			type = yyjson_get_str(type_val);
		} else {
			return StringUtil::Format("TransformTerm property 'type' is not of type 'string', found '%s' instead",
			                          yyjson_get_type_desc(type_val));
		}
	}
	auto transform_val = yyjson_obj_get(obj, "transform");
	if (!transform_val) {
		return "TransformTerm required property 'transform' is missing";
	} else {
		error = transform.TryFromJSON(transform_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto term_val = yyjson_obj_get(obj, "term");
	if (!term_val) {
		return "TransformTerm required property 'term' is missing";
	} else {
		error = term.TryFromJSON(term_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
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
