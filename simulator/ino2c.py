#!/usr/bin/env python3
import os
import re
import sys
from typing import List, Tuple


def find_decls(ino: str) -> Tuple[List[str], List[str]]:
    with open(ino, encoding="utf8") as f:
        txt = f.read()
        incls = re.findall(r"^#include.*$", txt, re.MULTILINE)
        decls = re.findall(
            r"^(?:(?:static|struct)[^;=]+?{|typedef[^;]+?;)", txt, re.MULTILINE
        )
        return incls, decls


def add_header(ino: str, c: str, incls: List[str], decls: List[str]) -> None:
    header = "\n".join(
        ['#include <arduino.h>', ""]
        + [" ".join(incl.split()) for incl in incls]
        + [" ".join(decl.rstrip("{;").split()) + ";" for decl in decls]
    )
    with open(ino, "r", encoding="utf8") as f:
        txt = f.read()
    with open(c, "w", encoding="utf8") as f:
        txt = header + "\n\n" + txt
        txt = txt.replace('#include "', '#include "../')
        f.write(txt)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: ino2c input.ino [output.cpp]")
    ino = sys.argv[1]
    c = (
        sys.argv[2]
        if len(sys.argv) >= 3
        else os.path.basename(os.path.splitext(ino)[0]) + ".cpp"
    )

    try:
        incls, decls = find_decls(ino)
        add_header(ino, c, incls, decls)
    except Exception as e:  # pylint: disable=broad-except
        sys.exit(e)
