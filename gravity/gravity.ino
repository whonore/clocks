#include <Adafruit_SSD1351.h>
#include <DS3231.h>

// Must be defined before including TimerInterrupt.h
#define USE_TIMER_1 true
#include <TimerInterrupt.h>

#include "clock.h"
#include "font.h"

//       CS  DC  MOSI SCLK RST MOT1 MOT2 MOT3 MOT4 ZERO ZOFF       MAX
static hand_t hand_min = \
    HAND(16, 15, 18,  17,  14, 26,  27,  28,  29,  52,  MIN_ZOFF,  MIN_MAX);
static hand_t hand_hour = \
    HAND(4,  5,  2,   3,   6,  22,  23,  24,  25,  53,  HOUR_ZOFF, HOUR_MAX);
#if DISPLAY_SEC
static int8_t last_sec = -1;
#endif

static DS3231 rtc(SDA, SCL);

void setup(void) {
    Serial.begin(9600);

    hand_t *HANDS[] = {&hand_min, &hand_hour};
    for (uint8_t i = 0; i < ARRAY_LEN(HANDS); i++) {
        HANDS[i]->screen.begin();
        HANDS[i]->screen.setRotation(ROTATE);
        HANDS[i]->screen.fillScreen(BG);
        HANDS[i]->motor.setMaxSpeed(SPEED_STEPS_PER_SEC);
        HANDS[i]->motor.setAcceleration(ACCEL_STEPS_PER_SEC);
        pinMode(HANDS[i]->zero_pin, INPUT_PULLUP);
        zero_hand(HANDS[i]);
    }

    rtc.begin();

    ITimer1.init();
    if (!ITimer1.attachInterruptInterval(MOTOR_INTERRUPT_MS, motor_handler)) {
        hand_min.err = hand_hour.err = true;
    }
}

void loop(void) {
    Time time = rtc.getTime();
    set_time(&hand_min, time.min);
    set_time(&hand_hour, time.hour);
#if DISPLAY_SEC
    if (last_sec == -1) {
        for (uint8_t sec = 0; sec < time.sec; sec++) {
            set_sec(&hand_min, sec);
        }
    }
    set_sec(&hand_min, time.sec);
#endif
}

// Step the motor back to the zero point.
static void zero_hand(struct hand_t *hand) {
    // Is the switch already pressed?
    if (digitalRead(hand->zero_pin) == LOW) {
        hand->motor.move(40);
        hand->motor.runToPosition();
    }

    // Step backwards until the switch is pressed or a full revolution.
    uint16_t steps;
    for (steps = 0; steps < 360; steps++) {
        if (digitalRead(hand->zero_pin) == LOW) {
            break;
        }
        hand->motor.move(-1);
        hand->motor.runToPosition();
        delay(50);
    }

    // Check if the switch was pressed within one revolution.
    hand->err = (steps == 360);

    // Adjust for the switch offset.
    if (!hand->err) {
        hand->motor.move(hand->zero_off);
        hand->motor.runToPosition();
        hand->motor.setCurrentPosition(0);
    }
}

// Move the motors if necessary. Triggered every `MOTOR_INTERRUPT_MS`
// milliseconds.
static void motor_handler(void) {
    hand_min.motor.run();
    hand_hour.motor.run();
}

// Check if `time` is different than what `hand` is currently displaying and,
// if so, update the display and move the motor.
static void set_time(struct hand_t *hand, uint8_t time) {
    time = time % hand->max_time;
    if (!hand->err && hand->time != time) {
        hand->time = time;

        hand->motor.moveTo(time * STEPS_PER_TIME(hand));

        clear_screen(hand);
        draw_time(hand->bitmap, time, hand->max_time);
        hand->screen.drawBitmap(0, 0, hand->bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, FG);
    }
}

// Set the bits in `bitmap` corresponding to the digits in `val`.
static void draw_time(uint8_t *bitmap, uint8_t val, uint8_t max) {
    if (max == HOUR_MAX && val == 0) {
        val = HOUR_MAX;
    }

    uint8_t hi = val / 10;
    uint8_t lo = val % 10;
    double angle = ANGLE_OF(val, max);

    memset(bitmap, 0, BITMAP_SZ);
    draw_digit(bitmap, hi, true, angle);
    draw_digit(bitmap, lo, false, angle);
}

// Set the bits in `bitmap` corresponding to `digit` rotated by `angle` in
// either the left or right half of the screen.
static void draw_digit(uint8_t *bitmap, uint8_t digit, bool left, double angle) {
    Point pt;

    for (size_t i = 0; i < font_len[digit]; i++) {
        // Load the next point from the font.
        // NOTE: Coordinates are relative to the top left of the image,
        // positive y is down on the screen.
        memcpy_P(pt, font[digit][i], sizeof(Point));

        // Scale the point.
        pt[0] *= SCALE;
        pt[1] *= SCALE;

        // Translate the point halfway down the screen and DIGIT_GAP / 2 from
        // the screen center.
        uint8_t x_off = left
            ? SCREEN_CENTER_X - IMG_WIDTH - (DIGIT_GAP / 2)
            : SCREEN_CENTER_X + (DIGIT_GAP / 2);
        uint8_t y_off = SCREEN_CENTER_Y - (IMG_HEIGHT / 2);
        pt[0] += x_off;
        pt[1] += y_off;

        // Adjust the coordinates relative to the screen origin ((0, 0) is the
        // screen center, positive y is up on the screen).
        double x = pt[0] - SCREEN_CENTER_X;
        double y = SCREEN_CENTER_Y - pt[1];

        // Rotate the point around the screen center.
        double x_rot = x * cos(angle) - y * sin(angle);
        double y_rot = x * sin(angle) + y * cos(angle);

        // Reset the coordinate reference frame ((0, 0) is the top left of the
        // screen, positive y is down on the screen).
        x_rot += SCREEN_CENTER_X;
        y_rot = SCREEN_CENTER_Y - y_rot;

        // Round to the nearest integer point and skip if it is out of bounds.
        if (round(x_rot) < 0 || round(y_rot) < 0) {
            continue;
        }
        pt[0] = round(x_rot);
        pt[1] = round(y_rot);

        // Draw `SCALE` pixels per point in each axis.
        for (uint8_t x_off = 0; x_off < SCALE; x_off++) {
            pt[0] += x_off;
            for (uint8_t y_off = 0; y_off < SCALE; y_off++) {
                pt[1] += y_off;
                draw_point(bitmap, pt);
                pt[1] -= y_off;
            }
            pt[0] -= x_off;
        }
    }
}

// Set the bit in `bitmap` corresponding to `pt`.
static void draw_point(uint8_t *bitmap, const Point pt) {
    if (pt[0] < SCREEN_WIDTH && pt[1] < SCREEN_WIDTH) {
        uint16_t idx;
        uint8_t bit;
        xy2bit(pt[0], pt[1], idx, bit);
        bitSet(bitmap[idx], bit);
    }
}

// Clear `hand`'s display.
static void clear_screen(struct hand_t *hand) {
    uint8_t x, y, width, height;
    find_bounding(hand->bitmap, &x, &y, &width, &height);
    hand->screen.fillRect(x, y, width, height, BG);
}

// Find the minimum bounding box for `bitmap`.
static void find_bounding(const uint8_t *bitmap, uint8_t *x, uint8_t *y,
                          uint8_t *width, uint8_t *height) {
    uint8_t min_x = 0;
    uint8_t min_y = 0;
    uint8_t max_x = 255;
    uint8_t max_y = 255;

    for (uint16_t idx = 0; idx < BITMAP_SZ; idx++) {
        for (int8_t bit = 7; bit >= 0; bit--) {
            if (bitRead(bitmap[idx], bit)) {
                uint8_t x = 0, y = 0;
                bit2xy(bit, idx, x, y);
                if (x < min_x) { min_x = x; }
                if (y < min_y) { min_y = y; }
                if (x > max_x) { max_x = x; }
                if (y > max_y) { max_y = y; }
            }
        }
    }

    *x = min_x;
    *y = min_y;
    *width = max_x - min_x;
    *height = max_y - min_y;
}

#if DISPLAY_SEC
// Check if `sec` is different than what `hand` is currently displaying and, if
// so, update the display.
static void set_sec(struct hand_t *hand, uint8_t sec) {
    if (!hand->err && last_sec != sec) {
        double angle = ANGLE_OF(hand->time, MIN_MAX);

        // Compute the previous and next points.
        Point pt, tgt_pt;
        enum Edge edge = draw_sec(tgt_pt, sec, angle);
        if (last_sec != -1) {
            draw_sec(pt, last_sec, angle);
        } else {
            memcpy(pt, tgt_pt, sizeof(Point));
        }

        // Fill in all points from `pt` to `tgt_pt`.
        do {
            if (pt[0] < tgt_pt[0] && pt[1] <= tgt_pt[1]) {
                pt[0] += 1;
                edge = TOP;
            } else if (pt[0] > tgt_pt[0] && pt[1] >= tgt_pt[1]) {
                pt[0] -= 1;
                edge = BOTTOM;
            } else if (pt[1] < tgt_pt[1]) {
                pt[1] += 1;
                edge = RIGHT;
            } else if (pt[1] > tgt_pt[1]) {
                pt[1] -= 1;
                edge = LEFT;
            }

            // Compute the point `SEC_THICKNESS` pixels away.
            Point pt_end;
            memcpy(pt_end, pt, sizeof(Point));
            switch (edge) {
                case TOP:
                    pt_end[1] += SEC_THICKNESS_OFF;
                    break;
                case BOTTOM:
                    pt_end[1] -= SEC_THICKNESS_OFF;
                    break;
                case RIGHT:
                    pt_end[0] -= SEC_THICKNESS_OFF;
                    break;
                case LEFT:
                    pt_end[0] += SEC_THICKNESS_OFF;
                    break;
            }

            hand->screen.drawLine(pt[0], pt[1], pt_end[0], pt_end[1], SEC_FG);
        } while (pt[0] != tgt_pt[0] || pt[1] != tgt_pt[1]);

        last_sec = sec;
    }
}

// Set `pt` to the next part of the second display.
static enum Edge draw_sec(Point pt, uint8_t sec, double angle) {
    // Compute the angle of `sec` relative to the hand's current rotation.
    angle = NORMALIZE_ANGLE(ANGLE_OF(sec, SEC_MAX) - angle);

    // Compute the distance to the edge of the screen at the given angle.
    double x;
    double y;
    enum Edge edge = EDGE_OF_ANGLE(angle);
    switch (edge) {
        case TOP:
            x = (SCREEN_WIDTH_R / 2) * tan(angle);
            y = (SCREEN_HEIGHT_R / 2) - SEC_MARGIN_TOP_R;
            break;
        case BOTTOM:
            x = -(SCREEN_WIDTH_R / 2) * tan(angle);
            y = -(SCREEN_HEIGHT_R / 2) + SEC_MARGIN_BOTTOM_R;
            break;
        case RIGHT:
            x = (SCREEN_WIDTH_R / 2) - SEC_MARGIN_RIGHT_R;
            y = (SCREEN_HEIGHT_R / 2) * tan((M_PI / 2) - angle);
            break;
        case LEFT:
            x = -(SCREEN_WIDTH_R / 2) + SEC_MARGIN_LEFT_R;
            y = -(SCREEN_HEIGHT_R / 2) * tan((M_PI / 2) - angle);
            break;
    }

    // Adjust the coordinate reference frame ((0, 0) is the top left of the
    // screen, positive y is down on the screen).
    x += SCREEN_CENTER_X;
    y = SCREEN_CENTER_Y - y;

    // Round to the nearest integer point.
    pt[0] = round(constrain(x, 0, SCREEN_WIDTH - 1));
    pt[1] = round(constrain(y, 0, SCREEN_HEIGHT - 1));

    return edge;
}
#endif
