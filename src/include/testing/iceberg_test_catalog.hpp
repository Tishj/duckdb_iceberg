#pragma once

namespace duckdb {
class ExtensionLoader;

//! Explicitly activated, in-process REST catalog for SQL tests.
void RegisterIcebergTestCatalog(ExtensionLoader &loader);
} // namespace duckdb
