#include <DS3231.h>

#define DIGIT(c)  (c == ' ' ? 0 : c - '0')
#define DIGIT2(s) (DIGIT(s[0]) * 10 + DIGIT(s[1]))

#define ORD_UPPER(c)    (c - 'A')
#define ORD_LOWER(c)    (c - 'a')
#define HASH_MONTH(mon) ((ORD_UPPER(mon[0]) + ORD_LOWER(mon[1]) + ORD_LOWER(mon[2])) % 21)
const uint8_t PAD = 255;
const uint8_t MONTH_TABLE[] = {
    [HASH_MONTH("Jun")] = 6,  // 0
    [HASH_MONTH("Jan")] = 1,  // 1
    PAD, PAD, PAD,
    [HASH_MONTH("Aug")] = 8,  // 5
    [HASH_MONTH("Nov")] = 11, // 6
    PAD,
    [HASH_MONTH("Mar")] = 3,  // 8
    [HASH_MONTH("Dec")] = 12, // 9
    [HASH_MONTH("Feb")] = 2,  // 10
    [HASH_MONTH("Apr")] = 4,  // 11
    PAD, PAD,
    [HASH_MONTH("Oct")] = 10, // 14
    [HASH_MONTH("May")] = 5,  // 15
    [HASH_MONTH("Sep")] = 9,  // 16
    PAD, PAD,
    [HASH_MONTH("Jul")] = 7,  // 19
};

#define EST_OFF (-8)
const uint8_t hour_offset = 24 + EST_OFF;

// HH:MM:SS
const char time_s[9] = __TIME__;
const char hour_s[2] = {time_s[0], time_s[1]};
const char min_s[2] = {time_s[3], time_s[4]};
const char sec_s[2] = {time_s[6], time_s[7]};
const uint8_t hour = (DIGIT2(hour_s) + hour_offset) % 24;
const uint8_t min = DIGIT2(min_s);
const uint8_t sec = DIGIT2(sec_s);

// NOTE: Doesn't handle date offset.
// Mon DD YYYY
const char date_s[12] = __DATE__;
const char mon_s[3] = {date_s[0], date_s[1], date_s[2]};
const char day_s[2] = {date_s[4], date_s[5]};
const char year_s[2] = {date_s[9], date_s[10]};
const uint8_t mon = MONTH_TABLE[HASH_MONTH(mon_s)];
const uint8_t day = DIGIT2(day_s);
const uint16_t year = DIGIT2(year_s) + 2000;

static DS3231 rtc(SDA, SCL);

void setup(void) {
    rtc.begin();
    rtc.setTime(hour, min, sec);
    rtc.setDate(day, mon, year);
    Serial.begin(9600);
}

void loop(void) {
    Serial.println(rtc.getTimeStr());
    Serial.println(rtc.getDateStr());
    delay(5000);
}
