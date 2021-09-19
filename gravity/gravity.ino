#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

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

static const uint16_t BITMAP_SZ = (SCREEN_WIDTH * SCREEN_HEIGHT) / 8;
static byte sec_bitmap[BITMAP_SZ];

static byte secs = 0;

void setup(void) {
    Serial.begin(9600);
    screen.begin();
    screen.setRotation(3);

    screen.fillScreen(BLACK);
}

void loop() {
    draw_time(sec_bitmap, secs, SEC_MAX);

    // TODO: find the bounding box of each digit and fillRect just that area
    screen.fillScreen(BLACK);
    screen.drawBitmap(0, 0, sec_bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    // TODO: temporary. Mark the center of the screen
    screen.drawPixel(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0xF800);

    secs = (secs + 1) % SEC_MAX;

    delay(500);
}

// Set the bits in `bitmap` corresponding to the digits in `val`.
static void draw_time(byte *bitmap, byte val, byte max) {
    byte hi = val / 10;
    byte lo = val % 10;
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
        // Load the next point from the font.
        // NOTE: Coordinates are relative to the top left of the image, positive y
        // is down on the screen.
        memcpy_P(pt, font[digit][i], sizeof(Point));

        // Scale the point.
        pt[0] *= SCALE;
        pt[1] *= SCALE;

        // Translate the point halfway down the screen and DIGIT_GAP / 2 from
        // the screen center.
        byte x_off = left
            ? (SCREEN_WIDTH / 2) - IMG_WIDTH - (DIGIT_GAP / 2)
            : (SCREEN_WIDTH / 2) + (DIGIT_GAP / 2);
        byte y_off = (SCREEN_HEIGHT / 2) - (IMG_HEIGHT / 2);
        pt[0] += x_off;
        pt[1] += y_off;

        // Adjust the coordinates relative to the screen origin ((0, 0) is the
        // screen center, positive y is up on the screen).
        double x = pt[0] - (SCREEN_WIDTH / 2);
        double y = (SCREEN_HEIGHT / 2) - pt[1];

        // Rotate the point around the screen center.
        double x_rot = x * cos(angle) - y * sin(angle);
        double y_rot = x * sin(angle) + y * cos(angle);

        // Reset the coordinate reference frame ((0, 0) is the top left of the
        // screen, positive y is down on the screen).
        x_rot += (SCREEN_WIDTH / 2);
        y_rot = (SCREEN_HEIGHT / 2) - y_rot;

        // Round to the nearest integer point and skip if it is out of bounds.
        if (round(x_rot) < 0 || round(y_rot) < 0) {
            continue;
        }
        pt[0] = round(x_rot);
        pt[1] = round(y_rot);

        // Draw `SCALE` pixels per point in each axis.
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
