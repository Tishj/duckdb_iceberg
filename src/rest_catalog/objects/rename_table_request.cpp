
#include "rest_catalog/objects/rename_table_request.hpp"

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

RenameTableRequest::RenameTableRequest(TableIdentifier source_p, TableIdentifier destination_p)
    : source(std::move(source_p)), destination(std::move(destination_p)) {
}
RenameTableRequest::RenameTableRequest(const RenameTableRequest &other)
    : source(other.source.Copy()), destination(other.destination.Copy()) {
}
RenameTableRequest::RenameTableRequest(RenameTableRequest &&other)
    : RenameTableRequest(static_cast<const RenameTableRequest &>(other)) {
}

RenameTableRequestBuilder::RenameTableRequestBuilder() {
}

RenameTableRequestBuilder &RenameTableRequestBuilder::SetSource(TableIdentifier value) {
	source_.emplace(std::move(value));
	has_source_ = true;
	return *this;
}

RenameTableRequestBuilder &RenameTableRequestBuilder::SetDestination(TableIdentifier value) {
	destination_.emplace(std::move(value));
	has_destination_ = true;
	return *this;
}

RenameTableRequest RenameTableRequestBuilder::Build() {
	if (!has_source_) {
		throw InvalidInputException("RenameTableRequest required property 'source' is missing");
	}
	if (!has_destination_) {
		throw InvalidInputException("RenameTableRequest required property 'destination' is missing");
	}
	auto result = RenameTableRequest(std::move(*source_), std::move(*destination_));
	auto error = result.Validate();
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

string RenameTableRequestBuilder::TryBuild(optional<RenameTableRequest> &result) {
	try {
		result.emplace(Build());
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

string RenameTableRequest::TryFromJSON(yyjson_val *obj, RenameTableRequestBuilder &builder) {
	try {
		auto source_val = yyjson_obj_get(obj, "source");
		if (!source_val) {
			throw InvalidInputException("RenameTableRequest required property 'source' is missing");
		} else {
			builder.SetSource(TableIdentifier::FromJSON(source_val));
		}
		auto destination_val = yyjson_obj_get(obj, "destination");
		if (!destination_val) {
			throw InvalidInputException("RenameTableRequest required property 'destination' is missing");
		} else {
			builder.SetDestination(TableIdentifier::FromJSON(destination_val));
		}
		return "";
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

RenameTableRequest RenameTableRequest::FromJSON(yyjson_val *obj) {
	RenameTableRequestBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return builder.Build();
}

RenameTableRequest RenameTableRequest::Copy() const {
	return RenameTableRequest(*this);
}

string RenameTableRequest::Validate() const {
	string error;
	error = source.Validate();
	if (!error.empty()) {
		return error;
	}
	error = destination.Validate();
	if (!error.empty()) {
		return error;
	}
	return "";
}

void RenameTableRequest::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize: source
	yyjson_mut_val *source_val = source.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "source", source_val);

	// Serialize: destination
	yyjson_mut_val *destination_val = destination.ToJSON(doc);
	yyjson_mut_obj_add_val(doc, obj, "destination", destination_val);
}

yyjson_mut_val *RenameTableRequest::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
