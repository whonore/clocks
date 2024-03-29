#ifndef CLOCK_H
#define CLOCK_H

// Use 24-hour time. Must come before util.h.
#define HOUR24 1

#include "config.h"
#include "util.h"

#if NEOPIXEL
#  include <Adafruit_NeoPixel.h>
#endif

// Time
#if !REALTIME
#  if TICK_UNIT == USECS
#    define TICK() micros()
#  elif TICK_UNIT == MSECS
#    define TICK() millis()
#  else
#    error "TICK_UNIT must be USECS or MSECS"
#  endif

typedef uint64_t ticks_t;
#  define TICKS_MAX         ((ticks_t) ((uint32_t) (-1)))
#  define CLOCK_DRIFT_TICKS (CLOCK_DRIFT_PER_MIL / (1000000 / TICK_UNIT))
#  define TICKS_PER_SEC     ((ticks_t) TICK_UNIT + CLOCK_DRIFT_TICKS)
#  define TICKS_PER_DAY     ((ticks_t) TICKS_PER_SEC * SEC_MAX * MIN_MAX * HOUR_MAX)
static_assert(TICKS_PER_DAY / TICKS_PER_SEC
                == (ticks_t) SEC_MAX * MIN_MAX * HOUR_MAX,
              "Ticks overflowed");
#endif

struct clock_time_t {
    byte sec;
    byte min;
    byte hour;
};

#define NSECS  6
#define NMINS  6
#define NHOURS 5

// LEDs
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
#  define RING(n, off) \
    { .colors = ((color_t[(n)]) {0}), .offset = (off), .nsegs = (n) }
#else
#  define RING(ps) \
    { .pins = (ps), .nsegs = ARRAY_LEN(ps) }
#endif

#endif /* CLOCK_H */
