#ifndef CLOCK_H
#define CLOCK_H

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

const uint8_t STEPS_PER_CIRCLE = 60;
const uint8_t STEPS_PER_QUADRANT = STEPS_PER_CIRCLE  / 4;
const double ANGLE_PER_STEP = -((2 * M_PI) / STEPS_PER_CIRCLE);

#define STEPS_OF(val, max) ((val * STEPS_PER_CIRCLE) / max)
#define ANGLE_OF(val, max) (STEPS_OF(val, max) * ANGLE_PER_STEP)

struct time_t {
    byte secs;
    byte mins;
    byte hours;
};

// [x, y]
typedef byte Point[2];

#endif /* CLOCK_H */
