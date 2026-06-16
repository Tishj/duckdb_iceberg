
#include "rest_catalog/objects/table_requirement.hpp"

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

TableRequirement::TableRequirement(optional<AssertCreate> assert_create_p,
                                   optional<AssertTableUUID> assert_table_uuid_p,
                                   optional<AssertRefSnapshotId> assert_ref_snapshot_id_p,
                                   optional<AssertLastAssignedFieldId> assert_last_assigned_field_id_p,
                                   optional<AssertCurrentSchemaId> assert_current_schema_id_p,
                                   optional<AssertLastAssignedPartitionId> assert_last_assigned_partition_id_p,
                                   optional<AssertDefaultSpecId> assert_default_spec_id_p,
                                   optional<AssertDefaultSortOrderId> assert_default_sort_order_id_p)
    : assert_create(std::move(assert_create_p)), assert_table_uuid(std::move(assert_table_uuid_p)),
      assert_ref_snapshot_id(std::move(assert_ref_snapshot_id_p)),
      assert_last_assigned_field_id(std::move(assert_last_assigned_field_id_p)),
      assert_current_schema_id(std::move(assert_current_schema_id_p)),
      assert_last_assigned_partition_id(std::move(assert_last_assigned_partition_id_p)),
      assert_default_spec_id(std::move(assert_default_spec_id_p)),
      assert_default_sort_order_id(std::move(assert_default_sort_order_id_p)) {
}

TableRequirementBuilder::TableRequirementBuilder() {
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertCreate(AssertCreate value) {
	assert_create_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertTableUuid(AssertTableUUID value) {
	assert_table_uuid_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertRefSnapshotId(AssertRefSnapshotId value) {
	assert_ref_snapshot_id_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertLastAssignedFieldId(AssertLastAssignedFieldId value) {
	assert_last_assigned_field_id_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertCurrentSchemaId(AssertCurrentSchemaId value) {
	assert_current_schema_id_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &
TableRequirementBuilder::SetAssertLastAssignedPartitionId(AssertLastAssignedPartitionId value) {
	assert_last_assigned_partition_id_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertDefaultSpecId(AssertDefaultSpecId value) {
	assert_default_spec_id_.emplace(std::move(value));
	return *this;
}

TableRequirementBuilder &TableRequirementBuilder::SetAssertDefaultSortOrderId(AssertDefaultSortOrderId value) {
	assert_default_sort_order_id_.emplace(std::move(value));
	return *this;
}

TableRequirement TableRequirementBuilder::Build() {
	auto result = TableRequirement(std::move(assert_create_), std::move(assert_table_uuid_),
	                               std::move(assert_ref_snapshot_id_), std::move(assert_last_assigned_field_id_),
	                               std::move(assert_current_schema_id_), std::move(assert_last_assigned_partition_id_),
	                               std::move(assert_default_spec_id_), std::move(assert_default_sort_order_id_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string TableRequirementBuilder::TryBuild(optional<TableRequirement> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableRequirement TableRequirement::FromJSON(yyjson_val *obj) {
	TableRequirementBuilder builder;
	do {
		try {
			builder.SetAssertCreate(AssertCreate::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertTableUuid(AssertTableUUID::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertRefSnapshotId(AssertRefSnapshotId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertLastAssignedFieldId(AssertLastAssignedFieldId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertCurrentSchemaId(AssertCurrentSchemaId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertLastAssignedPartitionId(AssertLastAssignedPartitionId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertDefaultSpecId(AssertDefaultSpecId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		try {
			builder.SetAssertDefaultSortOrderId(AssertDefaultSortOrderId::FromJSON(obj));
			break;
		} catch (const Exception &) {
		}
		throw InvalidInputException("TableRequirement failed to parse, none of the oneOf candidates matched");
	} while (false);
	return builder.Build();
}

string TableRequirement::TryFromJSON(yyjson_val *obj, optional<TableRequirement> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

TableRequirement TableRequirement::Copy() const {
	TableRequirementBuilder builder;
	optional<AssertCreate> assert_create_tmp;
	if (assert_create.has_value()) {
		assert_create_tmp.emplace((*assert_create).Copy());
	}
	if (assert_create_tmp.has_value()) {
		builder.SetAssertCreate(std::move(*assert_create_tmp));
	}
	optional<AssertTableUUID> assert_table_uuid_tmp;
	if (assert_table_uuid.has_value()) {
		assert_table_uuid_tmp.emplace((*assert_table_uuid).Copy());
	}
	if (assert_table_uuid_tmp.has_value()) {
		builder.SetAssertTableUuid(std::move(*assert_table_uuid_tmp));
	}
	optional<AssertRefSnapshotId> assert_ref_snapshot_id_tmp;
	if (assert_ref_snapshot_id.has_value()) {
		assert_ref_snapshot_id_tmp.emplace((*assert_ref_snapshot_id).Copy());
	}
	if (assert_ref_snapshot_id_tmp.has_value()) {
		builder.SetAssertRefSnapshotId(std::move(*assert_ref_snapshot_id_tmp));
	}
	optional<AssertLastAssignedFieldId> assert_last_assigned_field_id_tmp;
	if (assert_last_assigned_field_id.has_value()) {
		assert_last_assigned_field_id_tmp.emplace((*assert_last_assigned_field_id).Copy());
	}
	if (assert_last_assigned_field_id_tmp.has_value()) {
		builder.SetAssertLastAssignedFieldId(std::move(*assert_last_assigned_field_id_tmp));
	}
	optional<AssertCurrentSchemaId> assert_current_schema_id_tmp;
	if (assert_current_schema_id.has_value()) {
		assert_current_schema_id_tmp.emplace((*assert_current_schema_id).Copy());
	}
	if (assert_current_schema_id_tmp.has_value()) {
		builder.SetAssertCurrentSchemaId(std::move(*assert_current_schema_id_tmp));
	}
	optional<AssertLastAssignedPartitionId> assert_last_assigned_partition_id_tmp;
	if (assert_last_assigned_partition_id.has_value()) {
		assert_last_assigned_partition_id_tmp.emplace((*assert_last_assigned_partition_id).Copy());
	}
	if (assert_last_assigned_partition_id_tmp.has_value()) {
		builder.SetAssertLastAssignedPartitionId(std::move(*assert_last_assigned_partition_id_tmp));
	}
	optional<AssertDefaultSpecId> assert_default_spec_id_tmp;
	if (assert_default_spec_id.has_value()) {
		assert_default_spec_id_tmp.emplace((*assert_default_spec_id).Copy());
	}
	if (assert_default_spec_id_tmp.has_value()) {
		builder.SetAssertDefaultSpecId(std::move(*assert_default_spec_id_tmp));
	}
	optional<AssertDefaultSortOrderId> assert_default_sort_order_id_tmp;
	if (assert_default_sort_order_id.has_value()) {
		assert_default_sort_order_id_tmp.emplace((*assert_default_sort_order_id).Copy());
	}
	if (assert_default_sort_order_id_tmp.has_value()) {
		builder.SetAssertDefaultSortOrderId(std::move(*assert_default_sort_order_id_tmp));
	}
	return builder.Build();
}

string TableRequirement::Validate() const {
	string error;
	int matched_one_of_variants = 0;
	if (assert_create.has_value()) {
		matched_one_of_variants++;
		error = assert_create->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_table_uuid.has_value()) {
		matched_one_of_variants++;
		error = assert_table_uuid->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_ref_snapshot_id.has_value()) {
		matched_one_of_variants++;
		error = assert_ref_snapshot_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_last_assigned_field_id.has_value()) {
		matched_one_of_variants++;
		error = assert_last_assigned_field_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_current_schema_id.has_value()) {
		matched_one_of_variants++;
		error = assert_current_schema_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_last_assigned_partition_id.has_value()) {
		matched_one_of_variants++;
		error = assert_last_assigned_partition_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_default_spec_id.has_value()) {
		matched_one_of_variants++;
		error = assert_default_spec_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (assert_default_sort_order_id.has_value()) {
		matched_one_of_variants++;
		error = assert_default_sort_order_id->Validate();
		if (!error.empty()) {
			return error;
		}
	}
	if (matched_one_of_variants != 1) {
		return "TableRequirement must have exactly one oneOf variant set";
	}
	return "";
}

void TableRequirement::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	if (assert_create.has_value()) {
		assert_create->PopulateJSON(doc, obj);
	} else if (assert_table_uuid.has_value()) {
		assert_table_uuid->PopulateJSON(doc, obj);
	} else if (assert_ref_snapshot_id.has_value()) {
		assert_ref_snapshot_id->PopulateJSON(doc, obj);
	} else if (assert_last_assigned_field_id.has_value()) {
		assert_last_assigned_field_id->PopulateJSON(doc, obj);
	} else if (assert_current_schema_id.has_value()) {
		assert_current_schema_id->PopulateJSON(doc, obj);
	} else if (assert_last_assigned_partition_id.has_value()) {
		assert_last_assigned_partition_id->PopulateJSON(doc, obj);
	} else if (assert_default_spec_id.has_value()) {
		assert_default_spec_id->PopulateJSON(doc, obj);
	} else if (assert_default_sort_order_id.has_value()) {
		assert_default_sort_order_id->PopulateJSON(doc, obj);
	}
}

yyjson_mut_val *TableRequirement::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
