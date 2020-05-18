#define DEBUG 1
#if DEBUG
static char debug[256];
#define DPRINTF(...) sprintf(debug, __VA_ARGS__); Serial.print(debug)
#else
#define DPRINTF() do {} while(0)
#endif

#define ULONG_MAX ((unsigned long) (-1))
#define TIME_MS_MAX ((unsigned long) 1000 * 60 * 60 * 24)

// LEDs
const static int hours[] = {1, 2, 3, 4, 5};
#define NHOURS (sizeof(hours) / sizeof(hours[0]))
const static int mins[] = {6, 7, 8, 9, 10, 11};
#define NMINS (sizeof(mins) / sizeof(mins[0]))
const static int secs[] = {12, 13, 14, 15, 16, 17};
#define NSECS (sizeof(secs) / sizeof(secs[0]))

// Buttons
const static int incHour = 18;
const static int incMinute = 19;

static unsigned long last_ms = 0;
static unsigned long last_time = 0;
static int last_hour_st = LOW;
static int last_minute_st = LOW;
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
    pinMode(incMinute, INPUT);
}

void loop() {
    unsigned long time_ms = millis();
    unsigned long ellapsed =
        last_ms < time_ms ? time_ms - last_ms : time_ms + (ULONG_MAX - last_ms);
    unsigned int time[] = {off[0], off[1], 0};
    int hour_st = digitalRead(incHour);
    int minute_st = digitalRead(incMinute);

    if (1000 <= ellapsed) {
        last_time = (last_time + ellapsed) % TIME_MS_MAX;
        last_ms = time_ms;
        setTime(time, last_time);
        dispTime(hours, time[0], NHOURS);
        dispTime(mins, time[1], NMINS);
        dispTime(secs, time[2], NSECS);
        DPRINTF("H:%u\tM:%u\tS:%u\n", time[0], time[1], time[2]);
    }

    if (hour_st == HIGH && last_hour_st != HIGH) {
        DPRINTF("Hour+\n");
        off[0] = (off[0] + 1) % 24;
        delay(50);
    }

    if (minute_st == HIGH && last_minute_st != HIGH) {
        DPRINTF("Minute+\n");
        off[1] = (off[1] + 1) % 60;
        delay(50);
    }

    last_hour_st = hour_st;
    last_minute_st = minute_st;
}

static void setTime(unsigned int *time, unsigned long ms) {
    unsigned long sec = ms / 1000;
    unsigned long min = sec / 60;
    unsigned long hr = min / 60;
    time[0] = (time[0] + hr) % 24;
    time[1] = (time[1] + min) % 60;
    time[2] = (time[2] + sec) % 60;
}

static void dispTime(const int *leds, unsigned int val, unsigned int nleds) {
    for (unsigned int i = 0; i < nleds; i++) {
        digitalWrite(leds[i], bitRead(val, i) ? HIGH : LOW);
    }
}
