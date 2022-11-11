#include <DS3231.h>

#define DIGIT(c)  (c - '0')
#define DIGIT2(s) (DIGIT(s[0]) * 10 + DIGIT(s[1]))

// HH:MM:SS
const char time_s[9] = __TIME__;
const char hour_s[2] = {time_s[0], time_s[1]};
const char min_s[2] = {time_s[3], time_s[4]};
const char sec_s[2] = {time_s[6], time_s[7]};
const uint8_t hour = DIGIT2(hour_s);
const uint8_t min = DIGIT2(min_s);
const uint8_t sec = DIGIT2(sec_s);

static DS3231 rtc(SDA, SCL);

void setup(void) {
    rtc.begin();
    rtc.setTime(hour, min, sec);
}

void loop(void) {}
