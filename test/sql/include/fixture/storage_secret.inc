# Create the fixture storage secret once per database, before attaching.
# Requires httpfs. Does not attach a catalog or create schemas.
statement ok
CREATE SECRET (
    TYPE S3,
    KEY_ID 'admin',
    SECRET 'password',
    ENDPOINT '127.0.0.1:9000',
    URL_STYLE 'path',
    USE_SSL 0
);
