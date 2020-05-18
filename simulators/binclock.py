import math
import time
import turtle

def bits(n, max=None):
    len = math.ceil(math.log2(max)) if max is not None else 0
    return list(map(int, bin(n)[2:].zfill(len)))

def interpolate(min, max, n):
    step = math.ceil((max - min) / (n - 1))
    return list(range(min, max, step)) + [max]

class BinClock(turtle.Turtle):
    refresh_ms = 500
    rings = ('hour', 'min', 'sec')

    def __init__(self, dotsz=10, inner_radius=20, outer_radius=100, colors=None):
        self.dotsz = dotsz
        self.colors = {
            'hour': 'red',
            'min': 'blue',
            'sec': 'green'
        }
        if colors is not None:
            self.colors.update(colors)
        self.radii = interpolate(inner_radius, outer_radius, len(self.rings))
        super().__init__()

    def start(self):
        self.screen.mode('logo')
        self.screen.tracer(0, 0)
        self.hideturtle()
        self.penup()
        self.speed(0)
        self.draw()
        self.screen.mainloop()

    def draw(self):
        self.clear()
        for ridx, ring in enumerate(self.rings):
            time = getattr(self, ring)
            for bidx, bit in enumerate(reversed(time)):
                self.drawbit(self.radii[ridx], bidx, len(time), bit, self.colors[ring])
        self.screen.update()
        self.screen.ontimer(self.draw, self.refresh_ms)

    def drawbit(self, radius, idx, max, bit, color):
        if not bit:
            color = (0.8, 0.8, 0.8)
        self.home()
        self.setheading(idx * 360 / max)
        self.forward(radius)
        self.dot(self.dotsz, color)

    @property
    def hour(self):
        return bits(time.localtime().tm_hour, 24)

    @property
    def min(self):
        return bits(time.localtime().tm_min, 60)

    @property
    def sec(self):
        return bits(time.localtime().tm_sec, 60)

if __name__ == '__main__':
    BinClock().start()
