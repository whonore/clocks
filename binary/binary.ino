#include "clock.h"

// The state of each ring.
#if NEOPIXEL
static const ring_t secs = RING(NSECS, 0);
static const ring_t mins = RING(NMINS, NSECS);
static const ring_t hours = RING(NHOURS, NSECS + NMINS);
Adafruit_NeoPixel leds(NSECS + NMINS + NHOURS, 2, NEO_RGB + NEO_KHZ800);
#else
static constexpr ring_t secs = RING(((pin_t[NSECS]) {A0, A1, A2, A3, A4, A5}));
static_assert(secs.nsegs == NSECS, "Wrong number of secs");
static constexpr ring_t mins = RING(((pin_t[NMINS]) {2, 3, 4, 5, 6, 7}));
static_assert(mins.nsegs == NMINS, "Wrong number of mins");
static constexpr ring_t hours = RING(((pin_t[NHOURS]) {8, 9, 10, 11, 12}));
static_assert(hours.nsegs == NHOURS, "Wrong number of hours");
#endif

// The buttons to manually increment the minute and hour.
static const pin_t inc_hour = 0;
static const pin_t inc_min = 13;

// The previous voltage state read for each button.
static byte last_hour_st = HIGH;
static byte last_min_st = LOW;

// The previous (since the last loop) and total (modulo TICKS_PER_DAY) number
// of ticks.
static ticks_t prev_ticks = 0;
static ticks_t total_ticks = 0;

// The time offset to add to the Arduino's internal clock. Set by pressing the
// minute and hour buttons.
static struct time_t off = { .secs = 0, .mins = 0, .hours = 0 };

void setup() {
#if DEBUG
    Serial.begin(9600);
    while (!Serial) {}
#endif

    const ring_t rings[] = {secs, mins, hours};
#if NEOPIXEL
    const color_t colors[] = {
        SEC_COLOR1, SEC_COLOR2,
        MIN_COLOR1, MIN_COLOR2,
        HOUR_COLOR1, HOUR_COLOR2
    };
    leds.begin();
    leds.show();
#endif

    // Initialize the pins for each ring segment.
    for (byte r = 0; r < ARRAY_LEN(rings); r++) {
#if NEOPIXEL
        setColors(&rings[r], colors[2 * r], colors[2 * r + 1]);
#else
        for (byte i = 0; i < rings[r].nsegs; i++) {
            pinMode(rings[r].pins[i], OUTPUT);
        }
#endif
    }

    // Initialize the button pins.
    pinMode(inc_hour, INPUT);
    pinMode(inc_min, INPUT);

#if STARTUP
    // Display the startup animation.
    startup();
#endif
}

void loop() {
    // Compute the time difference in TICK_UNIT since the last loop, accounting
    // for overflow.
    ticks_t ticks = TICK();
    ticks_t ellapsed =
        prev_ticks < ticks ? ticks - prev_ticks : ticks + (TICKS_MAX - prev_ticks);

    // Check if a second has passed.
    bool at_second = (TICKS_PER_SEC <= ellapsed);

    // Check if either button was pressed.
    bool hour_pressed = pressed(inc_hour, &last_hour_st, LOW);
    bool min_pressed = pressed(inc_min, &last_min_st, HIGH);

    // Increment the hour.
    if (hour_pressed) {
        DPRINTF("Hour+\n");
        off.hours += 1;
        delay(100);
    }

    // Increment the minute.
    if (min_pressed) {
        DPRINTF("Minute+\n");
        off.mins += 1;
        delay(50);
    }

    // At least a second has passed so update the current time.
    if (at_second) {
        total_ticks = (total_ticks + ellapsed) % TICKS_PER_DAY;
        prev_ticks = ticks;
    }

    // Update the display.
    if (at_second || hour_pressed || min_pressed) {
        struct time_t time = off;
        ticksToTime(&time, total_ticks);
        dispTimes(time.secs, time.mins, time.hours);
        DPRINTF("H:%u\tM:%u\tS:%u\n", time.hours, time.mins, time.secs);
    }
}

// Was `button` pressed?
// True if its state matches `active` and differs from `old`.
// Also stores the new state in `old`.
static bool pressed(pin_t button, byte *old, byte active) {
    byte st = digitalRead(button);
    byte st_old = *old;
    *old = st;
    return (st == active && st_old != active);
}

// Convert `ticks` to seconds, minutes, and hours and add them to `time`.
static void ticksToTime(struct time_t *time, ticks_t ticks) {
    uint32_t sec = time->secs + (ticks / TICKS_PER_SEC);
    uint32_t min = time->mins + (sec / SEC_MAX);
    uint32_t hour = time->hours + (min / MIN_MAX);
    time->hours = hour % HOUR_MAX;
    time->mins = min % MIN_MAX;
    time->secs = sec % SEC_MAX;
}

// Update the ring segment LEDs.
static void dispTimes(byte sec, byte min, byte hour) {
#if NEOPIXEL
        leds.clear();
#endif
        dispTime(&secs, sec);
        dispTime(&mins, min);
        dispTime(&hours, hour);
#if NEOPIXEL
        leds.show();
#endif
}

// Update a single ring's LEDs.
static void dispTime(const ring_t *ring, byte val) {
    // Turn on or off each LED based on the corresponding bit of `val`.
    for (byte i = 0; i < ring->nsegs; i++) {
#if NEOPIXEL
        if (bitRead(val, i)) {
            leds.setPixelColor(ring->offset + i,
                               ring->colors[i][0],
                               ring->colors[i][1],
                               ring->colors[i][2]);
        }
#else
        digitalWrite(ring->pins[i], bitRead(val, i) ? HIGH : LOW);
#endif
    }
}

#if NEOPIXEL
// Compute the color for each segment by interpolating from `start` to `end`.
static void setColors(const struct ring_t *ring, const color_t start, const color_t end) {
    for (byte c = 0; c < 3; c++) {
        int step = abs(start[c] - end[c]) / (ring->nsegs - 1);
        step *= start[c] < end[c] ? 1 : -1;
        for (byte i = 0; i < ring->nsegs; i++) {
            ring->colors[i][c] = start[c] + step * i;
        }
    }
}
#endif

#if STARTUP
// Display an animation in which `nlights` consecutive segments spiral inwards
// around the rings.
static void startup() {
    const byte nlights = 4;
    const uint32_t mask = (1 << nlights) - 1;
    const uint32_t sec_mask = (1 << NSECS) - 1;
    const uint32_t min_mask = (1 << NMINS) - 1;
    const uint32_t hour_mask = (1 << NHOURS) - 1;

    // Shift a mask of `nlights` bits to the left.
    for (byte i = 1; i <= NSECS + NMINS + NHOURS; i++) {
        uint32_t time = i < nlights ? mask >> (nlights - i) : mask << (i - nlights);
        dispTimes(time & sec_mask,
                  (time >> NSECS) & min_mask,
                  (time >> (NSECS + NMINS)) & hour_mask);
        delay(100);
    }
}
#endif
