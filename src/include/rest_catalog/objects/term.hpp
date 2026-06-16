
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/reference.hpp"
#include "rest_catalog/objects/transform_term.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TermBuilder;

class Term {
public:
	Term(const Term &) = delete;
	Term &operator=(const Term &) = delete;
	Term(Term &&) = default;
	Term &operator=(Term &&) = delete;

private:
	friend class TermBuilder;
	Term(optional<Reference> reference_p, optional<TransformTerm> transform_term_p);

public:
	// Deserialization
	static Term FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, TermBuilder &builder);
	string Validate() const;

	// Copy
	Term Copy() const;

	// Serialization
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	optional<Reference> reference;
	optional<TransformTerm> transform_term;
};

class TermBuilder {
public:
	TermBuilder();
	TermBuilder &SetReference(Reference value);
	TermBuilder &SetTransformTerm(TransformTerm value);
	string TryBuild(optional<Term> &result);
	Term Build();

private:
	optional<Reference> reference_;
	optional<TransformTerm> transform_term_;
};

} // namespace rest_api_objects
} // namespace duckdb
