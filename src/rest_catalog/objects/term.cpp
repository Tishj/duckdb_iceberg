
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
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TermBuilder::TryBuild(optional<Term> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Term Term::FromJSON(yyjson_val *obj) {
	TermBuilder builder;
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
	return builder.Build();
}

string Term::TryFromJSON(yyjson_val *obj, optional<Term> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

Term Term::Copy() const {
	TermBuilder builder;
	optional<Reference> reference_tmp;
	if (reference.has_value()) {
		reference_tmp.emplace((*reference).Copy());
	}
	if (reference_tmp.has_value()) {
		builder.SetReference(std::move(*reference_tmp));
	}
	optional<TransformTerm> transform_term_tmp;
	if (transform_term.has_value()) {
		transform_term_tmp.emplace((*transform_term).Copy());
	}
	if (transform_term_tmp.has_value()) {
		builder.SetTransformTerm(std::move(*transform_term_tmp));
	}
	return builder.Build();
}

string Term::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (reference.has_value()) {
		matched_one_of_variants++;
		error = reference->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (transform_term.has_value()) {
		matched_one_of_variants++;
		error = transform_term->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "Term must have exactly one oneOf variant set";
	}
	return "";
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
