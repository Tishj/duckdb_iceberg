
#include "rest_catalog/objects/add_partition_spec_update.hpp"

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

AddPartitionSpecUpdate::AddPartitionSpecUpdate(BaseUpdate base_update_p, PartitionSpec spec_p)
    : base_update(std::move(base_update_p)), spec(std::move(spec_p)) {
}

AddPartitionSpecUpdateBuilder::AddPartitionSpecUpdateBuilder() {
}

AddPartitionSpecUpdateBuilder &AddPartitionSpecUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

AddPartitionSpecUpdateBuilder &AddPartitionSpecUpdateBuilder::SetSpec(PartitionSpec value) {
	spec_.emplace(std::move(value));
	has_spec_ = true;
	return *this;
}

AddPartitionSpecUpdate AddPartitionSpecUpdateBuilder::Build() {
	if (!has_spec_) {
		throw InvalidInputException("AddPartitionSpecUpdate required property 'spec' is missing");
	}
	auto result = AddPartitionSpecUpdate(std::move(*base_update_), std::move(*spec_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AddPartitionSpecUpdateBuilder::TryBuild(optional<AddPartitionSpecUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddPartitionSpecUpdate AddPartitionSpecUpdate::FromJSON(yyjson_val *obj) {
	AddPartitionSpecUpdateBuilder builder;
	builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
	auto spec_val = yyjson_obj_get(obj, "spec");
	if (!spec_val) {
		throw InvalidInputException("AddPartitionSpecUpdate required property 'spec' is missing");
	} else {
		builder.SetSpec(PartitionSpec::FromJSON(spec_val));
	}
	return builder.Build();
}

string AddPartitionSpecUpdate::TryFromJSON(yyjson_val *obj, optional<AddPartitionSpecUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AddPartitionSpecUpdate AddPartitionSpecUpdate::Copy() const {
	AddPartitionSpecUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	auto spec_tmp = spec.Copy();
	builder.SetSpec(std::move(spec_tmp));
	return builder.Build();
}

string AddPartitionSpecUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	error = spec.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void AddPartitionSpecUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: spec
	yyjson_mut_val *spec_val = spec.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "spec", spec_val);
}

yyjson_mut_val *AddPartitionSpecUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
