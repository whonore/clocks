#!/usr/bin/env python3
import re
import sys
from itertools import dropwhile, islice, takewhile
from typing import List, Tuple

DECL_MARKER = "!!!ino2c!!!"


def find_decls(infile: str) -> List[Tuple[str, str]]:
    with open(infile, encoding="utf8") as f:
        lines = islice(dropwhile(lambda line: DECL_MARKER not in line, f), 1, None)
        header = filter(
            lambda line: not line.startswith("#") and line.strip() != "",
            takewhile(lambda line: DECL_MARKER not in line, lines),
        )
        decls = [(line, re.search(r"\b(\w+)\(", line).group(1)) for line in header]  # type: ignore
        return decls


def remove_unused(infile: str, outfile: str, decls: List[Tuple[str, str]]) -> None:
    with open(infile, "r", encoding="utf8") as f:
        txt = f.read()
    for line, decl in decls:
        if len(re.findall(rf"\b{decl}\b", txt)) == 1:
            txt = txt.replace(line, "")
    with open(outfile, "w", encoding="utf8") as f:
        f.write(txt)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: remove-unused-decl input.cpp [output.cpp]")
    infile = sys.argv[1]
    outfile = sys.argv[2] if len(sys.argv) >= 3 else infile

    try:
        decls = find_decls(infile)
        remove_unused(infile, outfile, decls)
    except Exception as e:  # pylint: disable=broad-except
        sys.exit(e)
