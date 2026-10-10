"""Prepare the pinned DuckLake sources for this DuckDB revision without modifying the checkout."""

import argparse
from pathlib import Path


FIXES = {
    "src/storage/ducklake_delete.cpp": [
        ("MultiFileReader::FILENAME_FIELD_ID", "MultiFileReader::DELETE_FILE_PATH_FIELD_ID"),
        ("MultiFileReader::ORDINAL_FIELD_ID", "MultiFileReader::DELETE_POS_FIELD_ID"),
    ],
    "src/storage/statistics/ducklake_variant_stats.cpp": [
        (
            '\tif (path.size() == variant_field_start + 1 && path.back() == "metadata") {',
            "\t// DuckDB also reports statistics for the variant container itself.\n"
            "\t// Its metadata/value children below provide the usable statistics.\n"
            "\tif (path.size() >= variant_field_start && (path.size() - variant_field_start) % 2 == 0) {\n"
            "\t\treturn;\n"
            "\t}\n"
            '\tif (path.size() == variant_field_start + 1 && path.back() == "metadata") {',
        ),
    ],
}


def prepare(source: Path, destination: Path):
    source = source.resolve()
    destination = destination.resolve()
    if source == destination or source in destination.parents:
        raise ValueError("The generated DuckLake source directory must be separate from its checkout")
    files = [source / "CMakeLists.txt"]
    for folder in ("src", "test", "data"):
        files.extend(path for path in (source / folder).rglob("*") if path.is_file())
    for path in files:
        relative = path.relative_to(source)
        contents = path.read_bytes()
        if relative.as_posix() in FIXES:
            text = contents.decode()
            for old, new in FIXES[relative.as_posix()]:
                if new in text:
                    continue
                if text.count(old) != 1:
                    raise ValueError(f"Rebase the DuckLake compatibility fix for {relative}")
                text = text.replace(old, new)
            contents = text.encode()
        target = destination / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.exists() or target.read_bytes() != contents:
            target.write_bytes(contents)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    args = parser.parse_args()
    prepare(args.source, args.destination)
