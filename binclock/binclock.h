#ifndef BINCLOCK_H
#define BINCLOCK_H

#include "config.h"

// Util
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#if NEOPIXEL
#include <Adafruit_NeoPixel.h>
#endif

#if DEBUG
#define DEBUG_SZ 256
static char debug[DEBUG_SZ];
#define DPRINTF(...) snprintf(debug, DEBUG_SZ, __VA_ARGS__); Serial.print(debug)
#else
#define DPRINTF(...) do {} while (0)
#endif

// Time
#if TICK_UNIT == USECS
#define TICK() micros()
#elif TICK_UNIT == MSECS
#define TICK() millis()
#else
#error "TICK_UNIT must be USECS or MSECS"
#endif

typedef uint64_t ticks_t;
#define TICKS_MAX ((ticks_t) ((uint32_t) (-1)))
#define CLOCK_DRIFT_TICKS (CLOCK_DRIFT_PER_MIL / (1000000 / TICK_UNIT))
#define TICKS_PER_SEC ((ticks_t) TICK_UNIT + CLOCK_DRIFT_TICKS)
#define TICKS_PER_DAY ((ticks_t) TICKS_PER_SEC * 60 * 60 * 24)
static_assert(TICKS_PER_DAY / TICKS_PER_SEC == (ticks_t) 60 * 60 * 24,
              "Ticks overflowed");

struct time_t {
    byte secs;
    byte mins;
    byte hours;
};

#define NSECS 6
#define NMINS 6
#define NHOURS 5

// LEDs
typedef byte pin_t;
typedef byte color_t[3];

struct ring_t {
#if NEOPIXEL
    color_t *colors;
    const byte offset;
#else
    const pin_t *pins;
#endif
    const byte nsegs;
};

#if NEOPIXEL
#define RING(n, off) \
    { .colors = ((color_t[(n)]) {0}), .offset = (off), .nsegs = (n) }
#else
#define RING(ps) { .pins = (ps), .nsegs = ARRAY_LEN(ps) }
#endif

#endif /* BINCLOCK_H */
