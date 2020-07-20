#define DEBUG 0
#if DEBUG
static char debug[256];
#define DPRINTF(...) sprintf(debug, __VA_ARGS__); Serial.print(debug)
#else
#define DPRINTF(...) do {} while(0)
#endif

#define ULONG_MAX ((unsigned long) (-1))
#define TIME_MS_MAX ((unsigned long) 1000 * 60 * 60 * 24)

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

static unsigned long last_ms = 0;
static unsigned long last_time = 0;
static int last_hour_st = HIGH;
static int last_min_st = LOW;
static unsigned int off[] = {0, 0}; // Hour, minute

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
    unsigned long time_ms = millis();
    unsigned long ellapsed =
        last_ms < time_ms ? time_ms - last_ms : time_ms + (ULONG_MAX - last_ms);
    unsigned int time[] = {off[0], off[1], 0};
    bool hour_pressed = pressed(incHour, &last_hour_st, LOW);
    bool min_pressed = pressed(incMin, &last_min_st, HIGH);
    bool at_second = (1000 <= ellapsed);

    if (hour_pressed) {
        DPRINTF("Hour+\n");
        time[0] = off[0] = off[0] + 1;
        delay(100);
    }

    if (min_pressed) {
        DPRINTF("Minute+\n");
        time[1] = off[1] = off[1] + 1;
        delay(50);
    }

    if (at_second) {
        last_time = (last_time + ellapsed) % TIME_MS_MAX;
        last_ms = time_ms;
    }

    if (at_second || hour_pressed || min_pressed) {
        setTime(time, last_time);
        dispTime(hours, time[0], NHOURS);
        dispTime(mins, time[1], NMINS);
        dispTime(secs, time[2], NSECS);
        DPRINTF("H:%u\tM:%u\tS:%u\n", time[0], time[1], time[2]);
    }
}

static bool pressed(unsigned int button, unsigned int *old, unsigned int active) {
    int st = digitalRead(button);
    int st_old = *old;
    *old = st;
    return (st == active && st_old != active);
}

static void setTime(unsigned int *time, unsigned long ms) {
    unsigned long sec = time[2] + (ms / 1000);
    unsigned long min = time[1] + (sec / 60);
    unsigned long hr = time[0] + (min / 60);
    time[0] = hr % 24;
    time[1] = min % 60;
    time[2] = sec % 60;
}

static void dispTime(const int *leds, unsigned int val, unsigned int nleds) {
    for (unsigned int i = 0; i < nleds; i++) {
        digitalWrite(leds[i], bitRead(val, i) ? HIGH : LOW);
    }
}
