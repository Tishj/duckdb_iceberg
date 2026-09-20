# Default fixture setup, run once before the test body by init_sqllogic.
# Custom-setup tests compose these same includes with their own attach options.
include test/sql/init_requires

include test/sql/include/fixture/defaults.inc

include test/sql/include/fixture/storage_secret.inc

include test/sql/include/fixture/attach.inc

statement ok
CREATE SCHEMA IF NOT EXISTS {FIXTURE_CATALOG}.default;
