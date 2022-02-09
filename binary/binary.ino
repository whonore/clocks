#if REALTIME
#  include <DS3231.h>
#endif

#include "clock.h"

// The state of each ring.
#if NEOPIXEL
static const struct ring_t secs = RING(NSECS, 0);
static const struct ring_t mins = RING(NMINS, NSECS);
static const struct ring_t hours = RING(NHOURS, NSECS + NMINS);
Adafruit_NeoPixel leds(NSECS + NMINS + NHOURS, 2, NEO_RGB + NEO_KHZ800);
#else
static constexpr struct ring_t secs =
  RING(((pin_t[NSECS]) {A0, A1, A2, A3, A4, A5}));
static_assert(secs.nsegs == NSECS, "Wrong number of secs");
static constexpr struct ring_t mins = RING(((pin_t[NMINS]) {2, 3, 4, 5, 6, 7}));
static_assert(mins.nsegs == NMINS, "Wrong number of mins");
static constexpr struct ring_t hours =
  RING(((pin_t[NHOURS]) {8, 9, 10, 11, 12}));
static_assert(hours.nsegs == NHOURS, "Wrong number of hours");
#endif

// The buttons to manually increment the minute and hour.
static struct button_t inc_hour = {.pin = 0, .active = LOW, .state = HIGH};
static struct button_t inc_min = {.pin = 13, .active = HIGH, .state = LOW};

#if REALTIME
static DS3231 rtc(SDA, SCL);
static int8_t prev_sec = -1;
#else
// The previous (since the last loop) and total (modulo TICKS_PER_DAY) number
// of ticks.
static ticks_t prev_ticks = 0;
static ticks_t total_ticks = 0;
#endif

// The time offset to add to the Arduino's internal clock. Set by pressing the
// minute and hour buttons.
static struct clock_time_t off = {.sec = 0, .min = 0, .hour = 0};

void setup() {
#if DEBUG
    Serial.begin(9600);
    while (!Serial) {}
#endif

    const struct ring_t rings[] = {secs, mins, hours};
#if NEOPIXEL
    const color_t colors[] = {SEC_COLOR1,
                              SEC_COLOR2,
                              MIN_COLOR1,
                              MIN_COLOR2,
                              HOUR_COLOR1,
                              HOUR_COLOR2};
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
    pinMode(inc_hour.pin, INPUT);
    pinMode(inc_min.pin, INPUT);

#if REALTIME
    // Initialize the realtime clock.
    rtc.begin();
#endif

#if STARTUP
    // Display the startup animation.
    startup();
#endif
}

void loop() {
#if REALTIME
    Time rtime = rtc.getTime();
    bool at_second = rtime.sec != prev_sec;
#else
    // Compute the time difference in TICK_UNIT since the last loop, accounting
    // for overflow.
    ticks_t ticks = TICK();
    ticks_t ellapsed = prev_ticks < ticks ? ticks - prev_ticks
                                          : ticks + (TICKS_MAX - prev_ticks);
    // Check if a second has passed.
    bool at_second = (TICKS_PER_SEC <= ellapsed);
#endif

    // Check if either button was pressed.
    bool hour_pressed = pressed(&inc_hour);
    bool min_pressed = pressed(&inc_min);

    // Increment the hour.
    if (hour_pressed) {
        DPRINTF("Hour+\n");
        off.hour += 1;
        delay(100);
    }

    // Increment the minute.
    if (min_pressed) {
        DPRINTF("Minute+\n");
        off.min += 1;
        delay(50);
    }

    // At least a second has passed so update the current time.
    if (at_second) {
#if REALTIME
        prev_sec = rtime.sec;
#else
        total_ticks = (total_ticks + ellapsed) % TICKS_PER_DAY;
        prev_ticks = ticks;
#endif
    }

    // Update the display.
    if (at_second || hour_pressed || min_pressed) {
        struct clock_time_t time;
#if REALTIME
        time.sec = rtime.sec;
        time.min = rtime.min;
        time.hour = rtime.hour;
#else
        ticksToTime(&time, total_ticks);
#endif
        offsetTime(&time, &off);
        dispTimes(time.sec, time.min, time.hour);
        DPRINTF("H:%u\tM:%u\tS:%u\n", time.hour, time.min, time.sec);
    }
}

#if !REALTIME
// Convert `ticks` to seconds, minutes, and hours.
static void ticksToTime(struct clock_time_t *time, ticks_t ticks) {
    uint32_t sec = ticks / TICKS_PER_SEC;
    uint32_t min = sec / SEC_MAX;
    uint32_t hour = min / MIN_MAX;
    time->sec = sec % SEC_MAX;
    time->min = min % MIN_MAX;
    time->hour = hour % HOUR_MAX;
}
#endif

// Add `time` and `off`, storing the result in `time.
static void offsetTime(struct clock_time_t *time,
                       const struct clock_time_t *off) {
    uint32_t sec = time->sec + off->sec;
    uint32_t min = time->min + off->min + (sec / SEC_MAX);
    uint32_t hour = time->hour + off->hour + (min / MIN_MAX);
    time->sec = sec % SEC_MAX;
    time->min = min % MIN_MAX;
    time->hour = hour % HOUR_MAX;
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
static void dispTime(const struct ring_t *ring, byte val) {
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
static void setColors(const struct ring_t *ring,
                      const color_t start,
                      const color_t end) {
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
        uint32_t time =
          i < nlights ? mask >> (nlights - i) : mask << (i - nlights);
        dispTimes(time & sec_mask,
                  (time >> NSECS) & min_mask,
                  (time >> (NSECS + NMINS)) & hour_mask);
        delay(100);
    }
}
#endif
