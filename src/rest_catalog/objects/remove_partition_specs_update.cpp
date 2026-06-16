
#include "rest_catalog/objects/remove_partition_specs_update.hpp"

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

RemovePartitionSpecsUpdate::RemovePartitionSpecsUpdate(BaseUpdate base_update_p, vector<int32_t> spec_ids_p)
    : base_update(std::move(base_update_p)), spec_ids(std::move(spec_ids_p)) {
}
RemovePartitionSpecsUpdate::RemovePartitionSpecsUpdate(const RemovePartitionSpecsUpdate &other)
    : base_update(other.base_update.Copy()), spec_ids(([&]() {
	      vector<int32_t> copied;
	      copied.reserve(other.spec_ids.size());
	      for (const auto &item : other.spec_ids) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())) {
}
RemovePartitionSpecsUpdate::RemovePartitionSpecsUpdate(RemovePartitionSpecsUpdate &&other)
    : RemovePartitionSpecsUpdate(static_cast<const RemovePartitionSpecsUpdate &>(other)) {
}

RemovePartitionSpecsUpdateBuilder::RemovePartitionSpecsUpdateBuilder() {
}

RemovePartitionSpecsUpdateBuilder &RemovePartitionSpecsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

RemovePartitionSpecsUpdateBuilder &RemovePartitionSpecsUpdateBuilder::SetSpecIds(vector<int32_t> value) {
	spec_ids_.emplace(std::move(value));
	has_spec_ids_ = true;
	return *this;
}

RemovePartitionSpecsUpdate RemovePartitionSpecsUpdateBuilder::Build() {
	if (!has_spec_ids_) {
		throw InvalidInputException("RemovePartitionSpecsUpdate required property 'spec-ids' is missing");
	}
	auto result = RemovePartitionSpecsUpdate(std::move(*base_update_), std::move(*spec_ids_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemovePartitionSpecsUpdateBuilder::TryBuild(optional<RemovePartitionSpecsUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string RemovePartitionSpecsUpdate::TryFromJSON(yyjson_val *obj, RemovePartitionSpecsUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto spec_ids_val = yyjson_obj_get(obj, "spec-ids");
		if (!spec_ids_val) {
			throw InvalidInputException("RemovePartitionSpecsUpdate required property 'spec-ids' is missing");
		} else {
			vector<int32_t> spec_ids;
			if (yyjson_is_arr(spec_ids_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(spec_ids_val, idx, max, val) {
					int32_t tmp;
					if (yyjson_is_int(val)) {
						tmp = yyjson_get_int(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "RemovePartitionSpecsUpdate property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					spec_ids.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RemovePartitionSpecsUpdate property 'spec_ids' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(spec_ids_val)));
			}
			builder.SetSpecIds(std::move(spec_ids));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemovePartitionSpecsUpdate RemovePartitionSpecsUpdate::FromJSON(yyjson_val *obj) {
	RemovePartitionSpecsUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

RemovePartitionSpecsUpdate RemovePartitionSpecsUpdate::Copy() const {
	return RemovePartitionSpecsUpdate(*this);
}

string RemovePartitionSpecsUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void RemovePartitionSpecsUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: spec-ids
	yyjson_mut_val *spec_ids_arr = yyjson_mut_arr(doc);
	for (const auto &item : spec_ids) {
		yyjson_mut_val *item_val = yyjson_mut_int(doc, item);
		yyjson_mut_arr_append(spec_ids_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "spec-ids", spec_ids_arr);
}

yyjson_mut_val *RemovePartitionSpecsUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
