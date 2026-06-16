
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
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RemoveEncryptionKeyUpdateBuilder::TryBuild(optional<RemoveEncryptionKeyUpdate> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::FromJSON(yyjson_val *obj) {
	RemoveEncryptionKeyUpdateBuilder builder;
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
	return builder.Build();
}

string RemoveEncryptionKeyUpdate::TryFromJSON(yyjson_val *obj, optional<RemoveEncryptionKeyUpdate> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RemoveEncryptionKeyUpdate RemoveEncryptionKeyUpdate::Copy() const {
	RemoveEncryptionKeyUpdateBuilder builder;
	auto base_update_tmp = base_update.Copy();
	builder.SetBaseUpdate(std::move(base_update_tmp));
	string key_id_tmp;
	key_id_tmp = key_id;
	builder.SetKeyId(std::move(key_id_tmp));
	return builder.Build();
}

string RemoveEncryptionKeyUpdate::Validate() const {
	string error;
	error = base_update.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
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
