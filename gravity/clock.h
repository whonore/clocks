#ifndef CLOCK_H
#define CLOCK_H

#include <Adafruit_SSD1351.h>
#include <AccelStepper.h>

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

#if DISPLAY_SEC
// The offset for the inner edge of the second display is 1 pixel less than the
// `SEC_THICKNESS`.
const uint8_t SEC_THICKNESS_OFF = SEC_THICKNESS - 1;
#endif

// Compute the angle (in radians) at which to display `val` by computing what
// percent of a complete revolution it is (relative to `max`).
// NOTE: A positive angle corresponds to a counterclockwise rotation.
#define ANGLE_OF(val, max) ((val * 2 * M_PI) / max)

// Wrap `angle` into the [0, 2pi) range.
#define NORMALIZE_ANGLE(angle) \
    ((angle) - (2 * M_PI) * floor((angle) / (2 * M_PI)))

// Determine which edge a line projected from the screen center at `angle`
// intersects.
enum Edge {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
};
#define EDGE_OF_ANGLE(angle) ( \
    ((0 <= (angle) && (angle) <= M_PI / 4) \
     || (7 * (M_PI / 4) < (angle) && (angle) <= 2 * M_PI)) \
    ? TOP \
    : (M_PI / 4 < (angle) && (angle) <= 3 * (M_PI / 4)) \
    ? RIGHT \
    : (3 * (M_PI / 4) < (angle) && (angle) <= 5 * (M_PI / 4)) \
    ? BOTTOM \
    : LEFT)

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
    AccelStepper motor;
    const uint8_t max_time;
    const pin_t zero_pin;
    const int8_t zero_off;
    uint8_t bitmap[BITMAP_SZ];
    int8_t time;
    bool err;
};

#define HAND(cs, dc, mosi, sclk, rst, mot1, mot2, mot3, mot4, zero, zoff, max) \
    { \
        .screen = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc, mosi, sclk, rst), \
        .motor = AccelStepper(AccelStepper::FULL4WIRE, mot1, mot2, mot3, mot4), \
        .max_time = max, \
        .zero_pin = zero, \
        .zero_off = zoff, \
        .bitmap = {0}, \
        .time = -1, \
        .err = false, \
    }

// How many steps to take for each time increment to reach 360 degrees.
#define STEPS_PER_TIME(hand) (360 / (DEGREES_PER_STEP * (hand)->max_time))

#endif /* CLOCK_H */
