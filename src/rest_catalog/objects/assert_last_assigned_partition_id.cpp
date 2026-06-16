
#include "rest_catalog/objects/assert_last_assigned_partition_id.hpp"

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

AssertLastAssignedPartitionId::AssertLastAssignedPartitionId(TableRequirementType type_p,
                                                             int32_t last_assigned_partition_id_p)
    : type(std::move(type_p)), last_assigned_partition_id(std::move(last_assigned_partition_id_p)) {
}

AssertLastAssignedPartitionIdBuilder::AssertLastAssignedPartitionIdBuilder() {
}

AssertLastAssignedPartitionIdBuilder &AssertLastAssignedPartitionIdBuilder::SetType(TableRequirementType value) {
	type_.emplace(std::move(value));
	has_type_ = true;
	return *this;
}

AssertLastAssignedPartitionIdBuilder &AssertLastAssignedPartitionIdBuilder::SetLastAssignedPartitionId(int32_t value) {
	last_assigned_partition_id_.emplace(std::move(value));
	has_last_assigned_partition_id_ = true;
	return *this;
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionIdBuilder::Build() {
	if (!has_type_) {
		throw InvalidInputException("AssertLastAssignedPartitionId required property 'type' is missing");
	}
	if (!has_last_assigned_partition_id_) {
		throw InvalidInputException(
		    "AssertLastAssignedPartitionId required property 'last-assigned-partition-id' is missing");
	}
	auto result = AssertLastAssignedPartitionId(std::move(*type_), std::move(*last_assigned_partition_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string AssertLastAssignedPartitionIdBuilder::TryBuild(optional<AssertLastAssignedPartitionId> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string AssertLastAssignedPartitionId::TryFromJSON(yyjson_val *obj, AssertLastAssignedPartitionIdBuilder &builder) {
	try {
		auto type_val = yyjson_obj_get(obj, "type");
		if (!type_val) {
			throw InvalidInputException("AssertLastAssignedPartitionId required property 'type' is missing");
		} else {
			builder.SetType(TableRequirementType::FromJSON(type_val));
		}
		auto last_assigned_partition_id_val = yyjson_obj_get(obj, "last-assigned-partition-id");
		if (!last_assigned_partition_id_val) {
			throw InvalidInputException(
			    "AssertLastAssignedPartitionId required property 'last-assigned-partition-id' is missing");
		} else {
			int32_t last_assigned_partition_id;
			if (yyjson_is_int(last_assigned_partition_id_val)) {
				last_assigned_partition_id = yyjson_get_int(last_assigned_partition_id_val);
			} else {
				throw InvalidInputException(
				    StringUtil::Format("AssertLastAssignedPartitionId property 'last_assigned_partition_id' is not of "
				                       "type 'integer', found '%s' instead",
				                       yyjson_get_type_desc(last_assigned_partition_id_val)));
			}
			builder.SetLastAssignedPartitionId(std::move(last_assigned_partition_id));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionId::FromJSON(yyjson_val *obj) {
	AssertLastAssignedPartitionIdBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionId::Copy() const {
	AssertLastAssignedPartitionIdBuilder builder;
	auto type_tmp = type.Copy();
	builder.SetType(std::move(type_tmp));
	int32_t last_assigned_partition_id_tmp;
	last_assigned_partition_id_tmp = last_assigned_partition_id;
	builder.SetLastAssignedPartitionId(std::move(last_assigned_partition_id_tmp));
	return builder.Build();
}

string AssertLastAssignedPartitionId::Validate() const {
	string error;
	error = type.Validate();
	if (!error.empty()) {
		return error;
	}
	if (!StringUtil::CIEquals(type.value, "assert-last-assigned-partition-id")) {
		return StringUtil::Format(
		    "AssertLastAssignedPartitionId property 'type' must be assert-last-assigned-partition-id, not %s",
		    type.value);
	}
	return "";
}

void AssertLastAssignedPartitionId::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: type
	yyjson_mut_val *type_val = type.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "type", type_val);

	// Serialize: last-assigned-partition-id
	yyjson_mut_obj_add_int(doc, obj, "last-assigned-partition-id", last_assigned_partition_id);
}

yyjson_mut_val *AssertLastAssignedPartitionId::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
