
#pragma once

#include "yyjson.hpp"
#include "duckdb/common/optional.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/generated_object_access.hpp"
#include "rest_catalog/objects/reference.hpp"
#include "rest_catalog/objects/transform.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

class TransformTermBuilder;

class TransformTerm {
public:
	TransformTerm(const TransformTerm &);
	TransformTerm &operator=(const TransformTerm &) = delete;
	TransformTerm(TransformTerm &&);
	TransformTerm &operator=(TransformTerm &&) = delete;

private:
	friend class TransformTermBuilder;
	TransformTerm(string type_p, Transform transform_p, Reference term_p);

public:
	// Deserialization
	static TransformTerm FromJSON(yyjson_val *obj);
	static string TryFromJSON(yyjson_val *obj, TransformTermBuilder &builder);
	string Validate() const;

	// Copy
	TransformTerm Copy() const;

	// Serialization
	void PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const;
	yyjson_mut_val *ToJSON(yyjson_mut_doc *doc) const;

public:
	const string type;
	const Transform transform;
	const Reference term;
};

class TransformTermBuilder {
public:
	TransformTermBuilder();
	TransformTermBuilder &SetType(string value);
	TransformTermBuilder &SetTransform(Transform value);
	TransformTermBuilder &SetTerm(Reference value);
	string TryBuild(optional<TransformTerm> &result);
	TransformTerm Build();

private:
	optional<string> type_;
	optional<Transform> transform_;
	optional<Reference> term_;
	bool has_type_ = false;
	bool has_transform_ = false;
	bool has_term_ = false;
};

} // namespace rest_api_objects
} // namespace duckdb
