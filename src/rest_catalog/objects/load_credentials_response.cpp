
#include "rest_catalog/objects/load_credentials_response.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

LoadCredentialsResponse::LoadCredentialsResponse(vector<StorageCredential> storage_credentials_p)
    : storage_credentials(std::move(storage_credentials_p)) {
}

LoadCredentialsResponseBuilder::LoadCredentialsResponseBuilder() {
}

LoadCredentialsResponseBuilder &LoadCredentialsResponseBuilder::SetStorageCredentials(vector<StorageCredential> value) {
	storage_credentials_ = std::move(value);
	has_storage_credentials_ = true;
	return *this;
}

LoadCredentialsResponse LoadCredentialsResponseBuilder::Build() {
	if (!has_storage_credentials_) {
		throw InvalidInputException("LoadCredentialsResponse required property 'storage-credentials' is missing");
	}
	auto result = LoadCredentialsResponse(std::move(*storage_credentials_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string LoadCredentialsResponseBuilder::TryBuild(optional<LoadCredentialsResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LoadCredentialsResponse LoadCredentialsResponse::FromJSON(yyjson_val *obj) {
	LoadCredentialsResponseBuilder builder;
	auto storage_credentials_val = yyjson_obj_get(obj, "storage-credentials");
	if (!storage_credentials_val) {
		throw InvalidInputException("LoadCredentialsResponse required property 'storage-credentials' is missing");
	} else {
		vector<StorageCredential> storage_credentials;
		if (yyjson_is_arr(storage_credentials_val)) {
			size_t idx, max;
			yyjson_val *val;
			yyjson_arr_foreach(storage_credentials_val, idx, max, val) {
				auto tmp = StorageCredential::FromJSON(val);
				storage_credentials.emplace_back(std::move(tmp));
			}
		} else {
			return StringUtil::Format(
			    "LoadCredentialsResponse property 'storage_credentials' is not of type 'array', found '%s' instead",
			    yyjson_get_type_desc(storage_credentials_val));
		}
		builder.SetStorageCredentials(std::move(storage_credentials));
	}
	return builder.Build();
}

string LoadCredentialsResponse::TryFromJSON(yyjson_val *obj, optional<LoadCredentialsResponse> &result) {
	try {
		result.emplace(FromJSON(obj));
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

LoadCredentialsResponse LoadCredentialsResponse::Copy() const {
	LoadCredentialsResponseBuilder builder;
	vector<StorageCredential> storage_credentials_tmp;
	storage_credentials_tmp.reserve(storage_credentials.size());
	for (auto &item : storage_credentials) {
		storage_credentials_tmp.emplace_back(item.Copy());
	}
	builder.SetStorageCredentials(std::move(storage_credentials_tmp));
	return builder.Build();
}

string LoadCredentialsResponse::Validate() const {
	string error;
	for (const auto &item : storage_credentials) {
		error = item.Validate();
		if (!error.empty()) {
			return error;
		}
	}
	return "";
}

void LoadCredentialsResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: storage-credentials
	yyjson_mut_val *storage_credentials_arr = yyjson_mut_arr(doc);
	for (const auto &item : storage_credentials) {
		yyjson_mut_val *item_val = item.ToJSON(doc);
		yyjson_mut_arr_append(storage_credentials_arr, item_val);
	}
	yyjson_mut_obj_add_val(doc, obj, "storage-credentials", storage_credentials_arr);
}

yyjson_mut_val *LoadCredentialsResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
