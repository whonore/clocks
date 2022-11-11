#ifndef DS3231_H
#define DS3231_H

#include <time.h>

class Time {
  public:
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t date;
    uint8_t mon;
    uint16_t year;
    uint8_t dow;

    Time() {
        this->year = 2014;
        this->mon = 1;
        this->date = 1;
        this->hour = 0;
        this->min = 0;
        this->sec = 0;
        this->dow = 3;
    }
};

class DS3231 {
  public:
    DS3231(int8_t UNUSED(sda), int8_t UNUSED(scl)) {}

    void begin() {}

    void setTime(uint8_t UNUSED(hour),
                 uint8_t UNUSED(min),
                 uint8_t UNUSED(sec)) {}

    Time getTime() {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        struct tm *time = localtime(&ts.tv_sec);

        Time t;
        t.year = time->tm_year;
        t.mon = time->tm_mon;
        t.date = time->tm_mday;
        t.hour = time->tm_hour;
        t.min = time->tm_min;
        t.sec = time->tm_sec;
        t.dow = time->tm_wday;
        return t;
    }
};

#endif /* DS3231_H */
