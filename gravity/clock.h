#ifndef CLOCK_H
#define CLOCK_H

#include <Adafruit_SSD1351.h>
#include <Stepper.h>

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

const uint8_t IMG_WIDTH = RESOLUTION * SCALE;
const uint8_t IMG_HEIGHT = RESOLUTION * SCALE;

// Adjust so the screen center is constant regardless of rotation.
const bool SWAP_XY = ROTATE == 1 || ROTATE == 3;
const bool FLIP_X = ROTATE == 1 || ROTATE == 2;
const bool FLIP_Y = ROTATE == 2 || ROTATE == 3;
const uint8_t _SCREEN_CENTER_X =
  (SCREEN_WIDTH / 2) +
  (FLIP_X ? -1 : 1) *
  constrain(SCREEN_OFF_X, -(SCREEN_WIDTH / 2), (SCREEN_WIDTH / 2) - 1);
const uint8_t _SCREEN_CENTER_Y =
  (SCREEN_HEIGHT / 2) +
  (FLIP_Y ? -1 : 1) *
  constrain(SCREEN_OFF_Y, -(SCREEN_HEIGHT / 2), (SCREEN_HEIGHT / 2) - 1);
const uint8_t SCREEN_CENTER_X = SWAP_XY ? _SCREEN_CENTER_Y : _SCREEN_CENTER_X;
const uint8_t SCREEN_CENTER_Y = SWAP_XY ? _SCREEN_CENTER_X : _SCREEN_CENTER_Y;

const uint16_t BITMAP_SZ = (SCREEN_WIDTH * SCREEN_HEIGHT) / 8;

const uint8_t STEPS_PER_REV = 200;

// Compute the angle (in radians) at which to display `val` by computing what
// percent of a complete revolution it is (relative to `max`).
// NOTE: A positive angle corresponds to a counterclockwise rotation.
#define ANGLE_OF(val, max) ((val * 2 * M_PI) / max)

// Convert between (x, y) coordinates and bitmap (idx, bit) pairs.
#define _xy2idx(x, y) (y * SCREEN_WIDTH + x)
#define xy2bit(x, y, idx, bit) \
    (idx = _xy2idx(x, y) / 8, bit = 7 - (_xy2idx(x, y) % 8))
#define _bit2idx(bit, idx) (idx * 8 + (7 - bit))
#define bit2xy(idx, bit, x, y) \
    (x = _bit2idx(bit, idx) % SCREEN_WIDTH, y = _bit2idx(bit, idx) / SCREEN_WIDTH)

// Sanity check. Not supported by C++11.
#if __cpp_constexpr >= 201304
static constexpr bool _test_xy_bit() {
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t idx = 0;
    uint8_t bit = 0;
    xy2bit(SCREEN_CENTER_X, SCREEN_CENTER_Y, idx, bit);
    bit2xy(idx, bit, x, y);
    return (x == SCREEN_CENTER_X) && (y == SCREEN_CENTER_Y);
}
static_assert(_test_xy_bit(), "xy2bit and bit2xy are not inverses");
#endif

// [x, y]
typedef uint8_t Point[2];

struct hand_t {
    Adafruit_SSD1351 screen;
    Stepper motor;
    const uint8_t step_size;
    const pin_t zero_pin;
    const int8_t zero_off;
    uint8_t bitmap[BITMAP_SZ];
    int8_t time;
    bool err;
};

#define HAND(cs, dc, mosi, sclk, rst, mot1, mot2, mot3, mot4, step, zero, zoff) \
    { \
        .screen = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc, mosi, sclk, rst), \
        .motor = Stepper(STEPS_PER_REV, mot1, mot2, mot3, mot4), \
        .step_size = step, \
        .zero_pin = zero, \
        .zero_off = zoff, \
        .bitmap = {0}, \
        .time = -1, \
        .err = false, \
    }

#endif /* CLOCK_H */
