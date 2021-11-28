#ifndef UTIL_H
#define UTIL_H

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

typedef byte pin_t;

#ifndef HOUR24
#define HOUR24 1
#endif

#define SEC_MAX  60
#define MIN_MAX  60
#if HOUR24
#define HOUR_MAX 24
#else
#define HOUR_MAX 12
#endif

// Colors
const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xFFFF;
const uint16_t YELLOW = 0xFFE0;

#endif /* UTIL_H */
