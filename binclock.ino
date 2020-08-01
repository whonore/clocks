#define STATIC_ASSERT(name, test) typedef char assert_##name[(!!(test)) ? 1 : -1]

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define DEBUG_SZ 256
static char debug[DEBUG_SZ];
#define DPRINTF(...) snprintf(debug, DEBUG_SZ, __VA_ARGS__); Serial.print(debug)
#else
#define DPRINTF(...) do {} while (0)
#endif

#define USECS 1000000
#define MSECS 1000

#ifndef TICK_UNIT
#define TICK_UNIT USECS
#endif

#if TICK_UNIT == USECS
#define TICK() micros()
#elif TICK_UNIT == MSECS
#define TICK() millis()
#else
#error "TICK_UNIT must be USECS or MSECS"
#endif

typedef uint64_t ticks_t;
#define TICKS_MAX ((ticks_t) ((uint32_t) (-1)))
// How far from true TICK_UNIT the clock is.
// E.g. 0.15% slow = -1500/1000000 = -1.5ms offset.
#define CLOCK_DRIFT_PER_MIL (-1500)
#define CLOCK_DRIFT_TICKS (CLOCK_DRIFT_PER_MIL / (1000000 / TICK_UNIT))
#define TICKS_PER_SEC ((ticks_t) TICK_UNIT + CLOCK_DRIFT_TICKS)
#define TICKS_PER_DAY ((ticks_t) TICKS_PER_SEC * 60 * 60 * 24)
STATIC_ASSERT(ticks_correct, TICKS_PER_DAY / TICKS_PER_SEC == (ticks_t) 60 * 60 * 24);

struct time_t {
    byte secs;
    byte mins;
    byte hours;
};

typedef byte pin_t;

// LEDs
const static pin_t hours[] = {8, 9, 10, 11, 12};
#define NHOURS (sizeof(hours) / sizeof(hours[0]))
const static pin_t mins[] = {2, 3, 4, 5, 6, 7};
#define NMINS (sizeof(mins) / sizeof(mins[0]))
const static pin_t secs[] = {A0, A1, A2, A3, A4, A5};
#define NSECS (sizeof(secs) / sizeof(secs[0]))

// Buttons
const static pin_t incHour = 0;
const static pin_t incMin = 13;

static byte last_hour_st = HIGH;
static byte last_min_st = LOW;
static ticks_t prev_ticks = 0;
static ticks_t total_ticks = 0;
static struct time_t off = {.secs = 0, .mins = 0, .hours = 0};

void setup() {
#if DEBUG
    Serial.begin(9600);
    while (!Serial) {}
#endif
    for (byte i = 0; i < NHOURS; i++) {
        pinMode(hours[i], OUTPUT);
    }
    for (byte i = 0; i < NMINS; i++) {
        pinMode(mins[i], OUTPUT);
    }
    for (byte i = 0; i < NSECS; i++) {
        pinMode(secs[i], OUTPUT);
    }
    pinMode(incHour, INPUT);
    pinMode(incMin, INPUT);
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
        dispTime(hours, time.hours, NHOURS);
        dispTime(mins, time.mins, NMINS);
        dispTime(secs, time.secs, NSECS);
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

static void dispTime(const pin_t *leds, byte val, byte nleds) {
    for (byte i = 0; i < nleds; i++) {
        digitalWrite(leds[i], bitRead(val, i) ? HIGH : LOW);
    }
}
