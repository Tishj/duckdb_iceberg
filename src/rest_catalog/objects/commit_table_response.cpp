
#include "rest_catalog/objects/commit_table_response.hpp"

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

CommitTableResponse::CommitTableResponse(string metadata_location_p, TableMetadata metadata_p)
    : metadata_location(std::move(metadata_location_p)), metadata(std::move(metadata_p)) {
}
CommitTableResponse::CommitTableResponse(const CommitTableResponse &other)
    : metadata_location(other.metadata_location), metadata(other.metadata.Copy()) {
}
CommitTableResponse::CommitTableResponse(CommitTableResponse &&other)
    : CommitTableResponse(static_cast<const CommitTableResponse &>(other)) {
}

CommitTableResponseBuilder::CommitTableResponseBuilder() {
}

CommitTableResponseBuilder &CommitTableResponseBuilder::SetMetadataLocation(string value) {
	metadata_location_.emplace(std::move(value));
	has_metadata_location_ = true;
	return *this;
}

CommitTableResponseBuilder &CommitTableResponseBuilder::SetMetadata(TableMetadata value) {
	metadata_.emplace(std::move(value));
	has_metadata_ = true;
	return *this;
}

CommitTableResponse CommitTableResponseBuilder::Build() {
	if (!has_metadata_location_) {
		throw InvalidInputException("CommitTableResponse required property 'metadata-location' is missing");
	}
	if (!has_metadata_) {
		throw InvalidInputException("CommitTableResponse required property 'metadata' is missing");
	}
	auto result = CommitTableResponse(std::move(*metadata_location_), std::move(*metadata_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string CommitTableResponseBuilder::TryBuild(optional<CommitTableResponse> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string CommitTableResponse::TryFromJSON(yyjson_val *obj, CommitTableResponseBuilder &builder) {
	try {
		auto metadata_location_val = yyjson_obj_get(obj, "metadata-location");
		if (!metadata_location_val) {
			throw InvalidInputException("CommitTableResponse required property 'metadata-location' is missing");
		} else {
			string metadata_location;
			if (yyjson_is_str(metadata_location_val)) {
				metadata_location = yyjson_get_str(metadata_location_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "CommitTableResponse property 'metadata_location' is not of type 'string', found '%s' instead",
				    yyjson_get_type_desc(metadata_location_val)));
			}
			builder.SetMetadataLocation(std::move(metadata_location));
		}
		auto metadata_val = yyjson_obj_get(obj, "metadata");
		if (!metadata_val) {
			throw InvalidInputException("CommitTableResponse required property 'metadata' is missing");
		} else {
			builder.SetMetadata(TableMetadata::FromJSON(metadata_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

CommitTableResponse CommitTableResponse::FromJSON(yyjson_val *obj) {
	CommitTableResponseBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

CommitTableResponse CommitTableResponse::Copy() const {
	return CommitTableResponse(*this);
}

string CommitTableResponse::Validate() const {
	string error;
	error = metadata.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void CommitTableResponse::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: metadata-location
	yyjson_mut_obj_add_strcpy(doc, obj, "metadata-location", metadata_location.c_str());

	// Serialize: metadata
	yyjson_mut_val *metadata_val = metadata.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "metadata", metadata_val);
}

yyjson_mut_val *CommitTableResponse::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
