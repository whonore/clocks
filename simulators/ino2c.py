#!/usr/bin/env python3
import os
import re
import sys
from typing import List


def find_decls(ino: str) -> List[str]:
    with open(ino) as f:
        return re.findall(
            r"^(?:(?:static|struct)[^;=]+{|typedef[^;]+;)", f.read(), re.MULTILINE
        )


def add_header(ino: str, c: str, decls: List[str]) -> None:
    header = "\n".join(
        ['#include "arduino.h"', ""]
        + [" ".join(decl.rstrip("{;").split()) + ";" for decl in decls]
    )
    with open(ino, "r") as f:
        txt = re.sub(r"\bSerial\b(?!\.)", "Serial.Serial", f.read())
    with open(c, "w") as f:
        f.write(header + "\n\n" + txt)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: ino2c input.ino [output.c]")
    ino = sys.argv[1]
    c = (
        sys.argv[2]
        if len(sys.argv) >= 3
        else os.path.basename(os.path.splitext(ino)[0]) + ".c"
    )

    try:
        decls = find_decls(ino)
        add_header(ino, c, decls)
    except Exception as e:
        sys.exit(e)
