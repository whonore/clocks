#ifndef DST_H
#define DST_H

static const uint16_t EARLIEST_YEAR = 2025;
static constexpr uint8_t idx(uint16_t year) {
    return year - EARLIEST_YEAR;
}

// Pairs of start/end dates. The start is always in March, end is always in
// November.
// https://www.calendar-365.com/holidays/daylight-saving-%28start%29.html
// https://www.calendar-365.com/holidays/daylight-saving-%28end%29.html
static constexpr uint8_t DST[][2] = {
    { 9, 2}, // 2025
    { 8, 1}, // 2026
    {14, 7}, // 2027
    {12, 5}, // 2028
    {11, 4}, // 2029
    {10, 3}, // 2030
    { 9, 2}, // 2031
    {14, 6}, // 2032
    {13, 6}, // 2033
    {12, 5}, // 2034
    {11, 4}  // 2035
};

// NOTE: Ignores the hour and assumes DST starts at 00:00 in March and lasts
// through 23:59 in November.
constexpr bool is_dst(uint8_t day, uint8_t mon, uint16_t year) {
    return
        // Not between March and November: no
          (mon < 3 || mon > 11) ? false
        // Strictly between March and November: yes
        : (3 < mon && mon < 11) ? true
        // Year out of bounds: assume no
        : (idx(year) > sizeof(DST)) ? false
        // After start in March or before end in November: yes
        : ((mon == 3) && (day >= DST[idx(year)][0])) || ((mon == 11) && (day < DST[idx(year)][1]));
}

static_assert(is_dst(9, 3, 2025), "Mar 9 2025 should be DST");
static_assert(is_dst(10, 3, 2025), "Mar 10 2025 should be DST");
static_assert(is_dst(1, 4, 2025), "Apr 1 2025 should be DST");
static_assert(is_dst(1, 11, 2025), "Nov 1 2025 should be DST");
static_assert(is_dst(1, 8, 2026), "Mar 8 2026 should be DST");
static_assert(is_dst(1, 4, 3000), "Apr 1 3000 should be DST");
static_assert(!is_dst(2, 11, 2025), "Nov 2 2025 should not be DST");
static_assert(!is_dst(1, 1, 2025), "Jan 1 2025 should not be DST");
static_assert(!is_dst(1, 12, 2025), "Dec 1 2025 should not be DST");
static_assert(!is_dst(1, 1, 3000), "Jan 1 3000 should not be DST");
static_assert(!is_dst(1, 3, 3000), "Mar 1 3000 should not be DST");

#endif /* DST_H */
