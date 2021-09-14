import sys
from pathlib import Path
from typing import List, Sequence, Tuple

from PIL import Image


def show(data: Sequence[Tuple[int, int, int, int]], width: int) -> None:
    for idx, pix in enumerate(data):
        r, g, b, alph = pix
        print(
            "X" if r + g + b + alph != 0 else " ",
            end="\n" if (idx + 1) % width == 0 else "",
        )


def points(
    data: Sequence[Tuple[int, int, int, int]],
    width: int,
) -> List[Tuple[int, int]]:
    return [
        (idx % width, idx // width)
        for idx, pix in enumerate(data)
        if sum(pix) != 0
    ]


def convert(f: str, name: str) -> str:
    with Image.open(f) as im:
        data = im.getdata()
        pts = ", ".join(
            f"{{{x}, {y}}}" for x, y in points(list(data), im.width)
        )
        pts_name = f"pts_{name.split('-')[0]}"
        return (
            f'#include "../util.h"\n'
            f"const Point {pts_name}[] PROGMEM = {{\n    {pts}\n}};\n"
            f"const unsigned int {pts_name}_len = ARRAY_LEN({pts_name});"
        )


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit(f"Usage: {sys.argv[0]} file.png")
    name = Path(sys.argv[1]).stem
    print(convert(sys.argv[1], name))
