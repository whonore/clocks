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

#endif /* UTIL_H */
