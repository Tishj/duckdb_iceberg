
#include "rest_catalog/objects/assert_last_assigned_partition_id.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

AssertLastAssignedPartitionId::AssertLastAssignedPartitionId() {
}

AssertLastAssignedPartitionIdBuilder::AssertLastAssignedPartitionIdBuilder() {
}

AssertLastAssignedPartitionIdBuilder &AssertLastAssignedPartitionIdBuilder::SetType(TableRequirementType value) {
	result_.type = std::move(value);
	has_type_ = true;
	return *this;
}

AssertLastAssignedPartitionIdBuilder &AssertLastAssignedPartitionIdBuilder::SetLastAssignedPartitionId(int32_t value) {
	result_.last_assigned_partition_id = std::move(value);
	has_last_assigned_partition_id_ = true;
	return *this;
}

string AssertLastAssignedPartitionIdBuilder::TryBuild(AssertLastAssignedPartitionId &result) {
	if (!has_type_) {
		return "AssertLastAssignedPartitionId required property 'type' is missing";
	}
	if (!has_last_assigned_partition_id_) {
		return "AssertLastAssignedPartitionId required property 'last-assigned-partition-id' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionIdBuilder::Build() {
	AssertLastAssignedPartitionId result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionId::FromJSON(yyjson_val *obj) {
	AssertLastAssignedPartitionId res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

AssertLastAssignedPartitionId AssertLastAssignedPartitionId::Copy() const {
	AssertLastAssignedPartitionId res;
	res.type = type.Copy();
	res.last_assigned_partition_id = last_assigned_partition_id;
	return res;
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

string AssertLastAssignedPartitionId::TryFromJSON(yyjson_val *obj) {
	string error;
	auto type_val = yyjson_obj_get(obj, "type");
	if (!type_val) {
		return "AssertLastAssignedPartitionId required property 'type' is missing";
	} else {
		error = type.TryFromJSON(type_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto last_assigned_partition_id_val = yyjson_obj_get(obj, "last-assigned-partition-id");
	if (!last_assigned_partition_id_val) {
		return "AssertLastAssignedPartitionId required property 'last-assigned-partition-id' is missing";
	} else {
		if (yyjson_is_int(last_assigned_partition_id_val)) {
			last_assigned_partition_id = yyjson_get_int(last_assigned_partition_id_val);
		} else {
			return StringUtil::Format("AssertLastAssignedPartitionId property 'last_assigned_partition_id' is not of "
			                          "type 'integer', found '%s' instead",
			                          yyjson_get_type_desc(last_assigned_partition_id_val));
		}
	}
	return Validate();
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
