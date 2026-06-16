
#include "rest_catalog/objects/set_properties_update.hpp"

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

SetPropertiesUpdate::SetPropertiesUpdate(BaseUpdate base_update_p, case_insensitive_map_t<string> updates_p)
    : base_update(std::move(base_update_p)), updates(std::move(updates_p)) {
}
SetPropertiesUpdate::SetPropertiesUpdate(const SetPropertiesUpdate &other)
    : base_update(other.base_update.Copy()), updates(([&]() {
	      case_insensitive_map_t<string> copied;
	      for (const auto &entry : other.updates) {
		      copied.emplace(entry.first, entry.second);
	      }
	      return copied;
      }())) {
}
SetPropertiesUpdate::SetPropertiesUpdate(SetPropertiesUpdate &&other)
    : SetPropertiesUpdate(static_cast<const SetPropertiesUpdate &>(other)) {
}

SetPropertiesUpdateBuilder::SetPropertiesUpdateBuilder() {
}

SetPropertiesUpdateBuilder &SetPropertiesUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetPropertiesUpdateBuilder &SetPropertiesUpdateBuilder::SetUpdates(case_insensitive_map_t<string> value) {
	updates_.emplace(std::move(value));
	has_updates_ = true;
	return *this;
}

SetPropertiesUpdate SetPropertiesUpdateBuilder::Build() {
	if (!has_updates_) {
		throw InvalidInputException("SetPropertiesUpdate required property 'updates' is missing");
	}
	auto result = SetPropertiesUpdate(std::move(*base_update_), std::move(*updates_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> SetPropertiesUpdateBuilder::TryBuild(optional<SetPropertiesUpdate> &result) {
	if (!has_updates_) {
		return "SetPropertiesUpdate required property 'updates' is missing";
	}
	auto built = SetPropertiesUpdate(std::move(*base_update_), std::move(*updates_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> SetPropertiesUpdate::TryFromJSON(yyjson_val *obj, SetPropertiesUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto updates_val = yyjson_obj_get(obj, "updates");
		if (!updates_val) {
			throw InvalidInputException("SetPropertiesUpdate required property 'updates' is missing");
		} else {
			case_insensitive_map_t<string> updates;
			if (yyjson_is_obj(updates_val)) {
				size_t idx, max;
				yyjson_val *key, *val;
				yyjson_obj_foreach(updates_val, idx, max, key, val) {
					auto key_str = yyjson_get_str(key);
					string tmp;
					if (yyjson_is_str(val)) {
						tmp = yyjson_get_str(val);
					} else {
						throw InvalidInputException(StringUtil::Format(
						    "SetPropertiesUpdate property 'tmp' is not of type 'string', found '%s' instead",
						    yyjson_get_type_desc(val)));
					}
					updates.emplace(key_str, std::move(tmp));
				}
			} else {
				throw InvalidInputException("SetPropertiesUpdate property 'updates' is not of type 'object'");
			}
			builder.SetUpdates(std::move(updates));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetPropertiesUpdate SetPropertiesUpdate::FromJSON(yyjson_val *obj) {
	SetPropertiesUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

SetPropertiesUpdate SetPropertiesUpdate::Copy() const {
	return SetPropertiesUpdate(*this);
}

optional<string> SetPropertiesUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void SetPropertiesUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: updates
	yyjson_mut_val *updates_obj = yyjson_mut_obj(doc);
	for (const auto &it : updates) {
		auto &key = it.first;
		auto &value = it.second;
		auto key_ptr = unsafe_yyjson_mut_strncpy(doc, key.c_str(), strlen(key.c_str()));
		yyjson_mut_obj_add_strcpy(doc, updates_obj, key_ptr, value.c_str());
	}
	yyjson_mut_obj_add_val(doc, obj, "updates", updates_obj);
}

yyjson_mut_val *SetPropertiesUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
