#ifndef CLOCK_H
#define CLOCK_H

#include <Adafruit_SSD1351.h>

// Use 12-hour time. Must come before util.h.
#define HOUR24 0

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "config.h"

const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xFFFF;

const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 128;
const uint8_t IMG_WIDTH = RESOLUTION * SCALE;
const uint8_t IMG_HEIGHT = RESOLUTION * SCALE;
const uint16_t BITMAP_SZ = (SCREEN_WIDTH * SCREEN_HEIGHT) / 8;

static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) % 8 == 0,
              "SCREEN_WIDTH * SCREEN_HEIGHT must be a multiple of 8");

// Compute the angle (in radians) at which to display `val` by computing what
// percent of a complete revolution it is (relative to `max`).
// NOTE: A positive angle corresponds to a counterclockwise rotation.
#define ANGLE_OF(val, max) ((val * 2 * M_PI) / max)

// [x, y]
typedef byte Point[2];

struct screen_t {
    Adafruit_SSD1351 screen;
    byte bitmap[BITMAP_SZ];
    int8_t time;
};

#define SCREEN(cs, dc, mosi, sclk, rst) \
    { \
        .screen = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc, mosi, sclk, rst), \
        .bitmap = {0}, \
        .time = -1, \
    }

#endif /* CLOCK_H */
