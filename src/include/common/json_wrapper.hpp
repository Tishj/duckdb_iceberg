#pragma once

#include <type_traits>
#include <memory>
#include "yyjson.hpp"

using namespace duckdb_yyjson;

namespace duckdb {

struct YyjsonDocDeleter {
	void operator()(yyjson_doc *doc) {
		yyjson_doc_free(doc);
	}
	void operator()(yyjson_mut_doc *doc) {
		yyjson_mut_doc_free(doc);
	}
};

// Trait to check allowed types
template <typename T>
struct is_yyjson_doc : std::false_type {};

template <>
struct is_yyjson_doc<yyjson_doc> : std::true_type {};

template <>
struct is_yyjson_doc<yyjson_mut_doc> : std::true_type {};

// Alias with SFINAE
template <class T, class Enable = typename std::enable_if<is_yyjson_doc<T>::value>::type>
using json_unique_ptr = std::unique_ptr<T, YyjsonDocDeleter>;

string JsonToString(json_unique_ptr<yyjson_mut_doc> doc);

} // namespace duckdb
