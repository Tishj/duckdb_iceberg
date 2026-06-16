
#include "rest_catalog/objects/transform_term.hpp"

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

TransformTerm::TransformTerm(string type_p, Transform transform_p, Reference term_p)
    : type(std::move(type_p)), transform(std::move(transform_p)), term(std::move(term_p)) {
}
TransformTerm::TransformTerm(const TransformTerm &other)
    : type(other.type), transform(other.transform.Copy()), term(other.term.Copy()) {
}
TransformTerm::TransformTerm(TransformTerm &&other) : TransformTerm(static_cast<const TransformTerm &>(other)) {
}

TransformTermBuilder::TransformTermBuilder() {
}

TransformTermBuilder &TransformTermBuilder::SetType(string value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTransform(Transform value) {
	transform_.emplace(std::move(value));
	has_transform_ = true;
	return *this;
}

TransformTermBuilder &TransformTermBuilder::SetTerm(Reference value) {
	term_.emplace(std::move(value));
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
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> TransformTermBuilder::TryBuild(optional<TransformTerm> &result) {
	if (!has_type_) {
		return "TransformTerm required property 'type' is missing";
	}
	if (!has_transform_) {
		return "TransformTerm required property 'transform' is missing";
	}
	if (!has_term_) {
		return "TransformTerm required property 'term' is missing";
	}
	auto built = TransformTerm(std::move(*type_), std::move(*transform_), std::move(*term_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> TransformTerm::TryFromJSON(yyjson_val *obj, TransformTermBuilder &builder) {
	try {
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
			builder.SetTransform(Transform::FromJSON(transform_val));
		}
		auto term_val = yyjson_obj_get(obj, "term");
		if (!term_val) {
			throw InvalidInputException("TransformTerm required property 'term' is missing");
		} else {
			builder.SetTerm(Reference::FromJSON(term_val));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TransformTerm TransformTerm::FromJSON(yyjson_val *obj) {
	TransformTermBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

TransformTerm TransformTerm::Copy() const {
	return TransformTerm(*this);
}

optional<string> TransformTerm::Validate() const {
	optional<string> error;
	if (!StringUtil::CIEquals(type, "transform")) {
		return StringUtil::Format("TransformTerm property 'type' must be transform, not %s", type);
	}
	error = transform.Validate();
	if (error) {
		return error;
	}
	error = term.Validate();
	if (error) {
		return error;
	}
	return nullopt;
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
