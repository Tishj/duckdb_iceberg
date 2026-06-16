
#include "rest_catalog/objects/remove_encryption_key_update.hpp"

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

RemoveEncryptionKeyUpdate::RemoveEncryptionKeyUpdate(BaseUpdate base_update_p, string key_id_p)
    : base_update(std::move(base_update_p)), key_id(std::move(key_id_p)) {
}
RemoveEncryptionKeyUpdate::RemoveEncryptionKeyUpdate(const RemoveEncryptionKeyUpdate &other)
    : base_update(other.base_update.Copy()), key_id(other.key_id) {
}
RemoveEncryptionKeyUpdate::RemoveEncryptionKeyUpdate(RemoveEncryptionKeyUpdate &&other)
    : RemoveEncryptionKeyUpdate(static_cast<const RemoveEncryptionKeyUpdate &>(other)) {
}

RemoveEncryptionKeyUpdateBuilder::RemoveEncryptionKeyUpdateBuilder() {
}

RemoveEncryptionKeyUpdateBuilder &RemoveEncryptionKeyUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

RemoveEncryptionKeyUpdateBuilder &RemoveEncryptionKeyUpdateBuilder::SetKeyId(string value) {
	key_id_.emplace(std::move(value));
	has_key_id_ = true;
	return *this;
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdateBuilder::Build() {
	if (!has_key_id_) {
		throw InvalidInputException("RemoveEncryptionKeyUpdate required property 'key-id' is missing");
	}
	auto result = RemoveEncryptionKeyUpdate(std::move(*base_update_), std::move(*key_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> RemoveEncryptionKeyUpdateBuilder::TryBuild(optional<RemoveEncryptionKeyUpdate> &result) {
	if (!has_key_id_) {
		return "RemoveEncryptionKeyUpdate required property 'key-id' is missing";
	}
	auto built = RemoveEncryptionKeyUpdate(std::move(*base_update_), std::move(*key_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> RemoveEncryptionKeyUpdate::TryFromJSON(yyjson_val *obj, RemoveEncryptionKeyUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto key_id_val = yyjson_obj_get(obj, "key-id");
		if (!key_id_val) {
			throw InvalidInputException("RemoveEncryptionKeyUpdate required property 'key-id' is missing");
		} else {
			string key_id;
			if (yyjson_is_str(key_id_val)) {
				key_id = yyjson_get_str(key_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "RemoveEncryptionKeyUpdate property 'key_id' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(key_id_val)));
			}
			builder.SetKeyId(std::move(key_id));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::FromJSON(yyjson_val *obj) {
	RemoveEncryptionKeyUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::Copy() const {
	return RemoveEncryptionKeyUpdate(*this);
}

optional<string> RemoveEncryptionKeyUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void RemoveEncryptionKeyUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: key-id
	yyjson_mut_obj_add_strcpy(doc, obj, "key-id", key_id.c_str());
}

yyjson_mut_val *RemoveEncryptionKeyUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
