
#include "rest_catalog/objects/remove_snapshots_update.hpp"

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

RemoveSnapshotsUpdate::RemoveSnapshotsUpdate(BaseUpdate base_update_p, vector<int64_t> snapshot_ids_p)
    : base_update(std::move(base_update_p)), snapshot_ids(std::move(snapshot_ids_p)) {
}
RemoveSnapshotsUpdate::RemoveSnapshotsUpdate(const RemoveSnapshotsUpdate &other)
    : base_update(other.base_update.Copy()), snapshot_ids(([&]() {
	      vector<int64_t> copied;
	      copied.reserve(other.snapshot_ids.size());
	      for (const auto &item : other.snapshot_ids) {
		      copied.emplace_back(item);
	      }
	      return copied;
      }())) {
}
RemoveSnapshotsUpdate::RemoveSnapshotsUpdate(RemoveSnapshotsUpdate &&other)
    : RemoveSnapshotsUpdate(static_cast<const RemoveSnapshotsUpdate &>(other)) {
}

RemoveSnapshotsUpdateBuilder::RemoveSnapshotsUpdateBuilder() {
}

RemoveSnapshotsUpdateBuilder &RemoveSnapshotsUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

RemoveSnapshotsUpdateBuilder &RemoveSnapshotsUpdateBuilder::SetSnapshotIds(vector<int64_t> value) {
	snapshot_ids_.emplace(std::move(value));
	has_snapshot_ids_ = true;
	return *this;
}

RemoveSnapshotsUpdate RemoveSnapshotsUpdateBuilder::Build() {
	if (!has_snapshot_ids_) {
		throw InvalidInputException("RemoveSnapshotsUpdate required property 'snapshot-ids' is missing");
	}
	auto result = RemoveSnapshotsUpdate(std::move(*base_update_), std::move(*snapshot_ids_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> RemoveSnapshotsUpdateBuilder::TryBuild(optional<RemoveSnapshotsUpdate> &result) {
	if (!has_snapshot_ids_) {
		return "RemoveSnapshotsUpdate required property 'snapshot-ids' is missing";
	}
	auto built = RemoveSnapshotsUpdate(std::move(*base_update_), std::move(*snapshot_ids_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> RemoveSnapshotsUpdate::TryFromJSON(yyjson_val *obj, RemoveSnapshotsUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto snapshot_ids_val = yyjson_obj_get(obj, "snapshot-ids");
		if (!snapshot_ids_val) {
			throw InvalidInputException("RemoveSnapshotsUpdate required property 'snapshot-ids' is missing");
		} else {
			vector<int64_t> snapshot_ids;
			if (yyjson_is_arr(snapshot_ids_val)) {
				size_t idx, max;
				yyjson_val *val;
				yyjson_arr_foreach(snapshot_ids_val, idx, max, val) {
					int64_t tmp;
					if (yyjson_is_sint(val)) {
						tmp = yyjson_get_sint(val);
					} else if (yyjson_is_uint(val)) {
						tmp = yyjson_get_uint(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "RemoveSnapshotsUpdate property 'tmp' is not of type 'integer', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					snapshot_ids.emplace_back(std::move(tmp));
				}
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RemoveSnapshotsUpdate property 'snapshot_ids' is not of type 'array', found '%s' instead",
				    yyjson_get_type_desc(snapshot_ids_val)));
			}
			builder.SetSnapshotIds(std::move(snapshot_ids));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveSnapshotsUpdate RemoveSnapshotsUpdate::FromJSON(yyjson_val *obj) {
	RemoveSnapshotsUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

RemoveSnapshotsUpdate RemoveSnapshotsUpdate::Copy() const {
	return RemoveSnapshotsUpdate(*this);
}

optional<string> RemoveSnapshotsUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void RemoveSnapshotsUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: snapshot-ids
	yyjson_mut_val *snapshot_ids_arr = yyjson_mut_arr(doc);
	for (const auto &item : snapshot_ids) {
		yyjson_mut_val *item_val = yyjson_mut_sint(doc, item);
		yyjson_mut_arr_append(snapshot_ids_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "snapshot-ids", snapshot_ids_arr);
}

yyjson_mut_val *RemoveSnapshotsUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
