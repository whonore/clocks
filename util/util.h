#ifndef UTIL_H
#define UTIL_H

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

typedef byte pin_t;

// Maximum times
#ifndef HOUR24
#  define HOUR24 1
#endif

#define SEC_MAX  60
#define MIN_MAX  60
#define HOUR_MAX (HOUR24 ? 24 : 12)

// 16-bit Colors
const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xFFFF;
const uint16_t YELLOW = 0xFFE0;

// Debug printing
#if DEBUG
#  define DEBUG_SZ 256
static char debug[DEBUG_SZ];
#  define DPRINTF(...)                      \
    snprintf(debug, DEBUG_SZ, __VA_ARGS__); \
    Serial.print(debug)
#else
#  define DPRINTF(...) \
    do {               \
    } while (0)
#endif

// Buttons
struct button_t {
    const pin_t pin;
    const byte active;
    byte state;
};

// Was `button` pressed?
// True if its new state matches `button.active` and differs from
// `button.state`. Also stores the new state in `button.state`.
bool pressed(struct button_t *button) {
    byte st = digitalRead(button->pin);
    byte st_old = button->state;
    button->state = st;
    return (st == button->active && st_old != button->active);
}

#endif /* UTIL_H */
