#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include "clock.h"
#include "font.h"

static const uint8_t SCREEN_WIDTH = 128;
static const uint8_t SCREEN_HEIGHT = 128;
static const uint8_t IMG_WIDTH = RESOLUTION * SCALE;
static const uint8_t IMG_HEIGHT = RESOLUTION * SCALE;

static const pin_t sclk = 2;
static const pin_t mosi = 3;
static const pin_t dc = 4;
static const pin_t cs = 5;
static const pin_t rst = 6;
Adafruit_SSD1351 screen(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc, mosi, sclk, rst);

static const uint16_t BITMAP_SZ = (RESOLUTION * RESOLUTION) / 8;
static byte sec_bitmap[BITMAP_SZ];

static byte secs = 0;

void setup(void) {
    Serial.begin(9600);
    screen.begin();
    screen.setRotation(3);
}

void loop() {
    draw_time(sec_bitmap, secs, SEC_MAX);
    screen.drawBitmap(0, 0, sec_bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE, BLACK);
    secs = (secs + 1) % SEC_MAX;

    delay(1000);
}

// Set the bits in `bitmap` corresponding to the digits in `val`.
static void draw_time(byte *bitmap, byte val, byte max) {
    byte hi = val % 10;
    byte lo = val / 10;
    double angle = ANGLE_OF(val, max);
    memset(bitmap, 0, BITMAP_SZ);
    draw_digit(bitmap, hi, true, angle);
    draw_digit(bitmap, lo, false, angle);
}

// Set the bits in `bitmap` corresponding to `digit` rotated by `angle` in
// either the left or right half of the screen.
static void draw_digit(byte *bitmap, byte digit, bool left, double angle) {
    Point pt;

    for (size_t i = 0; i < font_len[digit]; i++) {
        // Load a point.
        memcpy_P(pt, font[digit][i], sizeof(Point));

        // Scale it.
        pt[0] *= SCALE;
        pt[1] *= SCALE;

        // Translate it relative to the screen center.
        byte x_off =
            // Shift the right edge DIGIT_GAP / 2 pixels from the center.
            ((SCREEN_WIDTH / 2) - IMG_WIDTH + (DIGIT_GAP / 2))
            // Move to the left or right half of the screen.
            + (left ? 0 : SCREEN_WIDTH / 2);
        byte y_off = SCREEN_HEIGHT / 2;
        pt[0] += x_off;
        pt[1] += y_off;

        // Rotate it around the screen center.
        double x = pt[0] * cos(angle) - pt[1] * sin(angle);
        double y = pt[0] * sin(angle) + pt[1] * cos(angle);
        pt[0] = round(x);
        pt[1] = round(y);

        // Draw SCALE pixels per point in each axis.
        for (byte x_off = 0; x_off < SCALE; x_off++) {
            pt[0] += x_off;
            for (byte y_off = 0; y_off < SCALE; y_off++) {
                pt[1] += y_off;
                draw_point(bitmap, pt);
                pt[1] -= y_off;
            }
            pt[0] -= x_off;
        }
    }
}

// Set the bit in `bitmap` corresponding to `pt`.
static void draw_point(byte *bitmap, Point pt) {
    if (pt[0] < SCREEN_WIDTH && pt[1] < SCREEN_WIDTH) {
        int xy = pt[1] * SCREEN_WIDTH + pt[0];
        int idx = xy / 8;
        int bit = 7 - (xy % 8);
        bitSet(bitmap[idx], bit);
    }
}
