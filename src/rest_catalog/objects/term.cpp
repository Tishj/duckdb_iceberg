
#include "rest_catalog/objects/term.hpp"

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

Term::Term(optional<Reference> reference_p, optional<TransformTerm> transform_term_p)
    : reference(std::move(reference_p)), transform_term(std::move(transform_term_p)) {
}
Term::Term(const Term &other)
    : reference((other.reference.has_value() ? optional<Reference>((*other.reference).Copy()) : optional<Reference>())),
      transform_term((other.transform_term.has_value() ? optional<TransformTerm>((*other.transform_term).Copy())
                                                       : optional<TransformTerm>())) {
}
Term::Term(Term &&other) : Term(static_cast<const Term &>(other)) {
}

TermBuilder::TermBuilder() {
}

TermBuilder &TermBuilder::SetReference(Reference value) {
	reference_.emplace(std::move(value));
	return *this;
}

TermBuilder &TermBuilder::SetTransformTerm(TransformTerm value) {
	transform_term_.emplace(std::move(value));
	return *this;
}

Term TermBuilder::Build() {
	auto result = Term(std::move(reference_), std::move(transform_term_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> TermBuilder::TryBuild(optional<Term> &result) {
	auto built = Term(std::move(reference_), std::move(transform_term_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> Term::TryFromJSON(yyjson_val *obj, TermBuilder &builder) {
	try {
		do {
			try {
				builder.SetReference(Reference::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			try {
				builder.SetTransformTerm(TransformTerm::FromJSON(obj));
				break;
			} catch (const Exception &) {
			}
			throw InvalidInputException("Term failed to parse, none of the oneOf candidates matched");
		} while (false);
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Term Term::FromJSON(yyjson_val *obj) {
	TermBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

Term Term::Copy() const {
	return Term(*this);
}

optional<string> Term::Validate() const {
	optional<string> error;
	int matched_one_of_variants = 0;
	if (reference.has_value()) {
		matched_one_of_variants++;
		error = reference->Validate();
		if (error) {
			return error;
		}
	}
	if (transform_term.has_value()) {
		matched_one_of_variants++;
		error = transform_term->Validate();
		if (error) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "Term must have exactly one oneOf variant set";
	}
	return nullopt;
}

yyjson_mut_val *Term::ToJSON(yyjson_mut_doc *doc) const {
	if (reference.has_value()) {
		return reference->ToJSON(doc);
	} else if (transform_term.has_value()) {
		return transform_term->ToJSON(doc);
	}
	// No variant is active - return empty object
	return yyjson_mut_obj(doc);
}

} // namespace rest_api_objects
} // namespace duckdb
