
#include "rest_catalog/objects/set_default_spec_update.hpp"

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

SetDefaultSpecUpdate::SetDefaultSpecUpdate(BaseUpdate base_update_p, int32_t spec_id_p)
    : base_update(std::move(base_update_p)), spec_id(std::move(spec_id_p)) {
}
SetDefaultSpecUpdate::SetDefaultSpecUpdate(const SetDefaultSpecUpdate &other)
    : base_update(other.base_update.Copy()), spec_id(other.spec_id) {
}
SetDefaultSpecUpdate::SetDefaultSpecUpdate(SetDefaultSpecUpdate &&other)
    : SetDefaultSpecUpdate(static_cast<const SetDefaultSpecUpdate &>(other)) {
}

SetDefaultSpecUpdateBuilder::SetDefaultSpecUpdateBuilder() {
}

SetDefaultSpecUpdateBuilder &SetDefaultSpecUpdateBuilder::SetBaseUpdate(BaseUpdate value) {
	base_update_.emplace(std::move(value));
	return *this;
}

SetDefaultSpecUpdateBuilder &SetDefaultSpecUpdateBuilder::SetSpecId(int32_t value) {
	spec_id_.emplace(std::move(value));
	has_spec_id_ = true;
	return *this;
}

SetDefaultSpecUpdate SetDefaultSpecUpdateBuilder::Build() {
	if (!has_spec_id_) {
		throw InvalidInputException("SetDefaultSpecUpdate required property 'spec-id' is missing");
	}
	auto result = SetDefaultSpecUpdate(std::move(*base_update_), std::move(*spec_id_));
	auto error = result.Validate();
	if (error) {
		throw InvalidInputException(*error);
	}
	return result;
}

optional<string> SetDefaultSpecUpdateBuilder::TryBuild(optional<SetDefaultSpecUpdate> &result) {
	if (!has_spec_id_) {
		return "SetDefaultSpecUpdate required property 'spec-id' is missing";
	}
	auto built = SetDefaultSpecUpdate(std::move(*base_update_), std::move(*spec_id_));
	auto error = built.Validate();
	if (error) {
		return error;
	}
	result.emplace(std::move(built));
	return nullopt;
}

optional<string> SetDefaultSpecUpdate::TryFromJSON(yyjson_val *obj, SetDefaultSpecUpdateBuilder &builder) {
	try {
		builder.SetBaseUpdate(BaseUpdate::FromJSON(obj));
		auto spec_id_val = yyjson_obj_get(obj, "spec-id");
		if (!spec_id_val) {
			throw InvalidInputException("SetDefaultSpecUpdate required property 'spec-id' is missing");
		} else {
			int32_t spec_id;
			if (yyjson_is_int(spec_id_val)) {
				spec_id = yyjson_get_int(spec_id_val);
			} else {
				throw InvalidInputException(StringUtil::Format(
				    "SetDefaultSpecUpdate property 'spec_id' is not of type 'integer', found '%s' instead",
				    yyjson_get_type_desc(spec_id_val)));
			}
			builder.SetSpecId(std::move(spec_id));
		}
		return nullopt;
	} catch (const Exception &ex) {
		auto error = ErrorData(ex);
		return error.RawMessage();
	}
}

SetDefaultSpecUpdate SetDefaultSpecUpdate::FromJSON(yyjson_val *obj) {
	SetDefaultSpecUpdateBuilder builder;
	auto error = TryFromJSON(obj, builder);
	if (error) {
		throw InvalidInputException(*error);
	}
	return builder.Build();
}

SetDefaultSpecUpdate SetDefaultSpecUpdate::Copy() const {
	return SetDefaultSpecUpdate(*this);
}

optional<string> SetDefaultSpecUpdate::Validate() const {
	optional<string> error;
	error = base_update.Validate();
	if (error) {
		return error;
	}
	return nullopt;
}

void SetDefaultSpecUpdate::PopulateJSON(yyjson_mut_doc *doc, yyjson_mut_val *obj) const {
	if (!yyjson_mut_is_obj(obj)) {
		throw InternalException("PopulateJSON requires obj to be a JSON object");
	}

	// Serialize base class: BaseUpdate
	base_update.PopulateJSON(doc, obj);

	// Serialize: spec-id
	yyjson_mut_obj_add_int(doc, obj, "spec-id", spec_id);
}

yyjson_mut_val *SetDefaultSpecUpdate::ToJSON(yyjson_mut_doc *doc) const {
	yyjson_mut_val *obj = yyjson_mut_obj(doc);
	PopulateJSON(doc, obj);
	return obj;
}

} // namespace rest_api_objects
} // namespace duckdb
