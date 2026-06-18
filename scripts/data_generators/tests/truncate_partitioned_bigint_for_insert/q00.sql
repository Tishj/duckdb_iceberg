DROP TABLE IF EXISTS default.truncate_partitioned_bigint_for_insert;
CREATE TABLE default.truncate_partitioned_bigint_for_insert (
    id INTEGER,
    value BIGINT
)
USING iceberg
PARTITIONED BY (truncate(value, 10))
