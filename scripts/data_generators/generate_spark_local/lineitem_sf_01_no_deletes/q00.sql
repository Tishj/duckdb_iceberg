CREATE or REPLACE TABLE iceberg_catalog.lineitem_sf_01_no_deletes
       TBLPROPERTIES (
        'format-version'='2',
        'write.update.mode'='merge-on-read'
       )
AS SELECT * FROM parquet_file_view;