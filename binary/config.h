#ifndef CONFIG_H
#define CONFIG_H

#include "local.h"

// Config
#ifndef NEOPIXEL
#  define NEOPIXEL 1
#endif

#ifndef DEBUG
#  define DEBUG 0
#endif

#ifndef STARTUP
#  define STARTUP 1
#endif

#define USECS 1000000
#define MSECS 1000

#ifndef TICK_UNIT
#  define TICK_UNIT USECS
#endif

// How far from true TICK_UNIT the clock is.
// E.g. 0.15% slow = -1500/1000000 = -1.5ms offset.
#define CLOCK_DRIFT_PER_MIL (-1500)

#if NEOPIXEL
#  define SEC_COLOR1 \
    { 0, 0, 255 }
#  define SEC_COLOR2 \
    { 100, 0, 100 }
#  define MIN_COLOR1 \
    { 0, 255, 0 }
#  define MIN_COLOR2 \
    { 0, 100, 100 }
#  define HOUR_COLOR1 \
    { 255, 0, 0 }
#  define HOUR_COLOR2 \
    { 100, 100, 0 }
#endif

#endif /* CONFIG_H */
