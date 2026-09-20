# Input: TABLE, a fully qualified SQL table identifier.
# Output: SQL variables latest_manifest_sequence and file_path (overwritten).
# Requires one ADDED data file in the latest manifest sequence.
# Does not open or close a transaction.

statement ok
set variable latest_manifest_sequence = (
	select max(manifest_sequence_number)
	from iceberg_metadata({TABLE})
);

query I
select count(*)
from iceberg_metadata({TABLE})
where manifest_sequence_number = cast(getvariable('latest_manifest_sequence') as bigint)
and manifest_content = 'DATA'
and status = 'ADDED';
----
1

statement ok
set variable file_path = (
	select file_path
	from iceberg_metadata({TABLE})
	where manifest_sequence_number = cast(getvariable('latest_manifest_sequence') as bigint)
	and manifest_content = 'DATA'
	and status = 'ADDED'
	limit 1
);

