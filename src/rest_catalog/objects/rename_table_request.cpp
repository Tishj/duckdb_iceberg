
#include "rest_catalog/objects/rename_table_request.hpp"

#include <regex>

#include "yyjson.hpp"
#include "duckdb/common/string.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/common/case_insensitive_map.hpp"
#include "rest_catalog/objects/list.hpp"

using namespace duckdb_yyjson;

namespace duckdb {
namespace rest_api_objects {

RenameTableRequest::RenameTableRequest()
    : source(GeneratedObjectAccess::Create<TableIdentifier>()),
      destination(GeneratedObjectAccess::Create<TableIdentifier>()) {
}

RenameTableRequestBuilder::RenameTableRequestBuilder() {
}

RenameTableRequestBuilder &RenameTableRequestBuilder::SetSource(TableIdentifier value) {
	result_.source = std::move(value);
	has_source_ = true;
	return *this;
}

RenameTableRequestBuilder &RenameTableRequestBuilder::SetDestination(TableIdentifier value) {
	result_.destination = std::move(value);
	has_destination_ = true;
	return *this;
}

string RenameTableRequestBuilder::TryBuild(RenameTableRequest &result) {
	if (!has_source_) {
		return "RenameTableRequest required property 'source' is missing";
	}
	if (!has_destination_) {
		return "RenameTableRequest required property 'destination' is missing";
	}
	auto error = result_.Validate();
	if (!error.empty()) {
		return error;
	}
	result = std::move(result_);
	return "";
}

RenameTableRequest RenameTableRequestBuilder::Build() {
	RenameTableRequest result;
	auto error = TryBuild(result);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return result;
}

RenameTableRequest RenameTableRequest::FromJSON(yyjson_val *obj) {
	RenameTableRequest res;
	auto error = res.TryFromJSON(obj);
	if (!error.empty()) {
		throw InvalidInputException(error);
	}
	return res;
}

RenameTableRequest RenameTableRequest::Copy() const {
	RenameTableRequest res;
	res.source = source.Copy();
	res.destination = destination.Copy();
	return res;
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

string RenameTableRequest::TryFromJSON(yyjson_val *obj) {
	string error;
	auto source_val = yyjson_obj_get(obj, "source");
	if (!source_val) {
		return "RenameTableRequest required property 'source' is missing";
	} else {
		error = source.TryFromJSON(source_val);
		if (!error.empty()) {
			return error;
		}
	}
	auto destination_val = yyjson_obj_get(obj, "destination");
	if (!destination_val) {
		return "RenameTableRequest required property 'destination' is missing";
	} else {
		error = destination.TryFromJSON(destination_val);
		if (!error.empty()) {
			return error;
		}
	}
	return Validate();
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
