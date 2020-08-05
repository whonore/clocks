#include "binclock.h"

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

// Buttons
static const pin_t incHour = 0;
static const pin_t incMin = 13;

static byte last_hour_st = HIGH;
static byte last_min_st = LOW;
static ticks_t prev_ticks = 0;
static ticks_t total_ticks = 0;
static struct time_t off = { .secs = 0, .mins = 0, .hours = 0 };

void setup() {
#if DEBUG
    Serial.begin(9600);
    while (!Serial) {}
#endif

    const ring_t rings[] = {secs, mins, hours};
#if NEOPIXEL
    const color_t colors[] = {
        {0, 0, 255}, {100, 0, 100}, // Seconds
        {0, 255, 0}, {0, 100, 100}, // Minutes
        {255, 0, 0}, {100, 100, 0}, // Hours
    };
    leds.begin();
    leds.show();
#endif

    for (byte r = 0; r < ARRAY_LEN(rings); r++) {
#if NEOPIXEL
        setColors(&rings[r], colors[2 * r], colors[2 * r + 1]);
#else
        for (byte i = 0; i < rings[r].nsegs; i++) {
            pinMode(rings[r].pins[i], OUTPUT);
        }
#endif
    }

    pinMode(incHour, INPUT);
    pinMode(incMin, INPUT);

#if STARTUP
    startup();
#endif
}

void loop() {
    struct time_t time;
    ticks_t ticks = TICK();
    ticks_t ellapsed =
        prev_ticks < ticks ? ticks - prev_ticks : ticks + (TICKS_MAX - prev_ticks);
    bool hour_pressed = pressed(incHour, &last_hour_st, LOW);
    bool min_pressed = pressed(incMin, &last_min_st, HIGH);
    bool at_second = (TICKS_PER_SEC <= ellapsed);

    if (hour_pressed) {
        DPRINTF("Hour+\n");
        off.hours += 1;
        delay(100);
    }

    if (min_pressed) {
        DPRINTF("Minute+\n");
        off.mins += 1;
        delay(50);
    }

    if (at_second) {
        total_ticks = (total_ticks + ellapsed) % TICKS_PER_DAY;
        prev_ticks = ticks;
    }

    if (at_second || hour_pressed || min_pressed) {
        time = off;
        ticksToTime(&time, total_ticks);
        dispTimes(time.secs, time.mins, time.hours);
        DPRINTF("H:%u\tM:%u\tS:%u\n", time.hours, time.mins, time.secs);
    }
}

static bool pressed(pin_t button, byte *old, byte active) {
    byte st = digitalRead(button);
    byte st_old = *old;
    *old = st;
    return (st == active && st_old != active);
}

static void ticksToTime(struct time_t *time, ticks_t ticks) {
    uint32_t sec = time->secs + (ticks / TICKS_PER_SEC);
    uint32_t min = time->mins + (sec / 60);
    uint32_t hr = time->hours + (min / 60);
    time->hours = hr % 24;
    time->mins = min % 60;
    time->secs = sec % 60;
}

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

static void dispTime(const ring_t *ring, byte val) {
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
static void setColors(const struct ring_t *ring, const color_t start, const color_t end) {
    int step;
    for (byte c = 0; c < 3; c++) {
        step = abs(start[c] - end[c]) / (ring->nsegs - 1);
        step *= start[c] < end[c] ? 1 : -1;
        for (byte i = 0; i < ring->nsegs; i++) {
            ring->colors[i][c] = start[c] + step * i;
        }
    }
}
#endif

#if STARTUP
static void startup() {
    const byte nlights = 4;
    const uint32_t mask = (1 << nlights) - 1;
    const uint32_t sec_mask = (1 << NSECS) - 1;
    const uint32_t min_mask = (1 << NMINS) - 1;
    const uint32_t hour_mask = (1 << NHOURS) - 1;
    uint32_t time;

    for (byte i = 1; i < NSECS + NMINS + NHOURS; i++) {
        time = i < nlights ? mask >> (nlights - i) : mask << (i - nlights);
        dispTimes(time & sec_mask,
                  (time >> NSECS) & min_mask,
                  (time >> (NSECS + NMINS)) & hour_mask);
        delay(50);
    }
}
#endif
