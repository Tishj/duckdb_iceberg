
#include "rest_catalog/objects/term.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

Term::Term() : transform_term(GeneratedObjectAccess::Create<optional<TransformTerm>>()) {
}

TermBuilder::TermBuilder() {
}

TermBuilder &TermBuilder::SetReference(Reference value) {
	result_.reference = std::move(value);
	return *this;
}

TermBuilder &TermBuilder::SetTransformTerm(TransformTerm value) {
	result_.transform_term = std::move(value);
	return *this;
}

string TermBuilder::TryBuild(Term &result) {
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

Term TermBuilder::Build() {
	Term result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

Term Term::FromJSON(yyjson_val *obj) {
	Term res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

Term Term::Copy() const {
	Term res;
	if (reference.has_value()) {
		res.reference.emplace();
		(*res.reference) = (*reference).Copy();
	}
	if (transform_term.has_value()) {
		res.transform_term = GeneratedObjectAccess::Create<TransformTerm>();
		(*res.transform_term) = (*transform_term).Copy();
	}
	return res;
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

string Term::TryFromJSON(yyjson_val *obj) {
	string error;
	do {
		reference.emplace();
		error = reference->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			reference = nullopt;
		}
		transform_term = GeneratedObjectAccess::Create<TransformTerm>();
		error = transform_term->TryFromJSON(obj);
		if (error.empty()) {
			break;
		} else {
			transform_term = nullopt;
		}
		return "Term failed to parse, none of the oneOf candidates matched";
	} while (false);
	return Validate();
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
