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

#define ULONG_MAX ((unsigned long) (-1))
// How far from true msecs the clock is. E.g. 0.2% slow = -2/1000 = -2ms offset.
#define CLOCK_DRIFT_PER_THOU (-2)
#define TICKS_PER_SEC (1000 + CLOCK_DRIFT_PER_THOU)
#define TICKS_MAX ((unsigned long) TICKS_PER_SEC * 60 * 60 * 24)

struct time_t {
    unsigned int secs;
    unsigned int mins;
    unsigned int hours;
};

// LEDs
const static int hours[] = {8, 9, 10, 11, 12};
#define NHOURS (sizeof(hours) / sizeof(hours[0]))
const static int mins[] = {2, 3, 4, 5, 6, 7};
#define NMINS (sizeof(mins) / sizeof(mins[0]))
const static int secs[] = {A0, A1, A2, A3, A4, A5};
#define NSECS (sizeof(secs) / sizeof(secs[0]))

// Buttons
const static int incHour = 0;
const static int incMin = 13;

static unsigned long prev_ticks = 0;
static unsigned long total_ticks = 0;
static int last_hour_st = HIGH;
static int last_min_st = LOW;
static struct time_t off = {.secs = 0, .mins = 0, .hours = 0};

void setup() {
#if DEBUG
    Serial.begin(9600);
    while (!Serial) {}
#endif
    for (unsigned int i = 0; i < NHOURS; i++) {
        pinMode(hours[i], OUTPUT);
    }
    for (unsigned int i = 0; i < NMINS; i++) {
        pinMode(mins[i], OUTPUT);
    }
    for (unsigned int i = 0; i < NSECS; i++) {
        pinMode(secs[i], OUTPUT);
    }
    pinMode(incHour, INPUT);
    pinMode(incMin, INPUT);
}

void loop() {
    struct time_t time;
    unsigned long ticks = millis();
    unsigned long ellapsed =
        prev_ticks < ticks ? ticks - prev_ticks : ticks + (ULONG_MAX - prev_ticks);
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
        total_ticks = (total_ticks + ellapsed) % TICKS_MAX;
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

static bool pressed(unsigned int button, int *old, int active) {
    int st = digitalRead(button);
    int st_old = *old;
    *old = st;
    return (st == active && st_old != active);
}

static void ticksToTime(struct time_t *time, unsigned long ticks) {
    unsigned long sec = time->secs + (ticks / TICKS_PER_SEC);
    unsigned long min = time->hours + (sec / 60);
    unsigned long hr = time->hours + (min / 60);
    time->hours = hr % 24;
    time->mins = min % 60;
    time->secs = sec % 60;
}

static void dispTime(const int *leds, unsigned int val, unsigned int nleds) {
    for (unsigned int i = 0; i < nleds; i++) {
        digitalWrite(leds[i], bitRead(val, i) ? HIGH : LOW);
    }
}
