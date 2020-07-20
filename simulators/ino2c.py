import os
import re
import sys
from typing import List


def find_statics(ino: str) -> List[str]:
    with open(ino) as f:
        return re.findall(r"static[^;=]+{", f.read())


def add_header(ino: str, c: str, funcs: List[str]) -> None:
    header = "\n".join(
        ['#include "arduino.h"', ""]
        + [" ".join(func.rstrip("{").split()) + ";" for func in funcs]
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
        funcs = find_statics(ino)
        add_header(ino, c, funcs)
    except Exception as e:
        sys.exit(e)
