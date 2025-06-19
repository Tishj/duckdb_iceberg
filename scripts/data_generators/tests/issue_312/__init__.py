from scripts.data_generators.tests.base import IcebergTest
from scripts.data_generators.connections.base import IcebergConnection
import pathlib
import tempfile
import duckdb
from glob import glob

PATH = pathlib.PurePath(__file__)


@IcebergTest.register()
class Test(IcebergTest):
    def __init__(self):
        super().__init__(PATH.parent.name)

        # Create a temporary directory
        self.tempdir = pathlib.Path(tempfile.mkdtemp())
        self.parquet_file = self.tempdir / "tmp.parquet"
        self.duckdb_con = duckdb.connect()

    def generate(self, con: IcebergConnection):
        self.setup(con)

    def setup(self, con):
        con.con.sql(
            """
            CREATE OR REPLACE TABLE default.test_filter(
                `id` string,
                `fetch_insertion_time` timestamp
            )
            USING iceberg
        """
        )
        insert_statement = """
            INSERT INTO default.test_filter SELECT * FROM parquet_file_view
        """

        res = glob(f'{PATH.parent}/*.parquet')
        for file in res:
            con.con.read.parquet(pathlib.Path(file).as_posix()).createOrReplaceTempView('parquet_file_view')
            con.con.sql(insert_statement)
