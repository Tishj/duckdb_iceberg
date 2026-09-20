# Load once before fixture setup; callers can override these tester parameters.
# WAREHOUSE is a SQL string literal; CATALOG is a SQL identifier. Other values
# are unquoted strings, except ATTACH_OPTIONS (a SQL fragment with leading comma).
set variable FIXTURE_WAREHOUSE ''

set variable FIXTURE_CATALOG my_datalake

set variable FIXTURE_CLIENT_ID admin

set variable FIXTURE_CLIENT_SECRET password

set variable FIXTURE_URI http://127.0.0.1:8181

# Capture an empty string: tester directives require a value token.
statement ok
SET VARIABLE fixture_attach_options = '';

set variable FIXTURE_ATTACH_OPTIONS <variable:fixture_attach_options>
