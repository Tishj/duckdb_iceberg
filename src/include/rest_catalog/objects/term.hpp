
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/reference.hpp"
#include "rest_catalog/objects/transform_term.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class Term {
public:
	Term();
	Term(const Term &) = delete;
	Term &operator=(const Term &) = delete;
	Term(Term &&) = default;
	Term &operator=(Term &&) = default;

public:
	// Deserialization
	static Term FromJSON(yyjson_val *obj);
	string TryFromJSON(yyjson_val *obj);
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
	string TryBuild(Term &result);
	Term Build();

private:
	Term result_;
};

} // namespace rest_api_objects
} // namespace duckdb
