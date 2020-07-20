import math
import time
import turtle
from typing import Mapping, Optional, Sequence, Tuple, Union

Color = Union[str, Tuple[float, float, float]]


def bits(n: int, max: Optional[int] = None) -> Sequence[int]:
    len = math.ceil(math.log2(max)) if max is not None else 0
    return list(map(int, bin(n)[2:].zfill(len)))


def interpolate(min: int, max: int, n: int) -> Sequence[int]:
    step = math.ceil((max - min) / (n - 1))
    return list(range(min, max, step)) + [max]


class BinClock(turtle.Turtle):
    refresh_ms = 500
    rings = ("hour", "min", "sec")

    def __init__(
        self,
        dotsz: int = 10,
        inner_radius: int = 39,
        outer_radius: int = 61,
        colors: Optional[Mapping[str, Color]] = None,
    ) -> None:
        self.dotsz = dotsz
        self.colors: Mapping[str, Color] = {
            "hour": "yellow",
            "min": "green",
            "sec": "blue",
        }
        if colors is not None:
            self.colors.update(colors)
        self.radii = interpolate(inner_radius, outer_radius, len(self.rings))
        super().__init__()

    def start(self) -> None:
        self.screen.mode("logo")  # type: ignore
        self.screen.tracer(0, 0)  # type: ignore
        self.screen.bgcolor("black")  # type: ignore
        self.hideturtle()
        self.penup()
        self.speed(0)
        self.draw()
        self.screen.mainloop()  # type: ignore

    def draw(self) -> None:
        self.clear()
        for ridx, ring in enumerate(self.rings):
            time = getattr(self, ring)
            for bidx, bit in enumerate(reversed(time)):
                self.drawbit(self.radii[ridx], bidx, len(time), bit, self.colors[ring])
        self.screen.update()  # type: ignore
        self.screen.ontimer(self.draw, self.refresh_ms)  # type: ignore

    def drawbit(self, radius: int, idx: int, max: int, bit: int, color: Color) -> None:
        if not bit:
            color = (0.8, 0.8, 0.8)
        self.home()
        self.setheading(idx * 360 / max)
        self.forward(radius)
        self.dot(self.dotsz, color)

    @property
    def hour(self) -> Sequence[int]:
        return bits(time.localtime().tm_hour, 24)

    @property
    def min(self) -> Sequence[int]:
        return bits(time.localtime().tm_min, 60)

    @property
    def sec(self) -> Sequence[int]:
        return bits(time.localtime().tm_sec, 60)


if __name__ == "__main__":
    BinClock().start()
