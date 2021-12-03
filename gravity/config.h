#ifndef CONFIG_H
#define CONFIG_H

#include "local.h"

// The number of steps to offset from the zero switch.
#ifndef MIN_ZOFF
#error "Must define MIN_ZOFF in local.h."
#endif
#ifndef HOUR_ZOFF
#error "Must define HOUR_ZOFF in local.h."
#endif

// The resolution of the digit font in pixels.
#ifndef RESOLUTION
#define RESOLUTION 16
#endif

// The screen dimensions in pixels.
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 128
#endif

static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) % 8 == 0,
              "SCREEN_WIDTH * SCREEN_HEIGHT must be a multiple of 8");

// How many pixels to shift the screen center (relative to ROTATE = 0).
#ifndef SCREEN_OFF_X
#define SCREEN_OFF_X -4
#endif
#ifndef SCREEN_OFF_Y
#define SCREEN_OFF_Y 11
#endif

// How large to draw each pixel.
#ifndef SCALE
#define SCALE 3
#endif

// The space between digits in pixels.
#ifndef DIGIT_GAP
#define DIGIT_GAP 16
#endif

// Multiple of 90 degrees to rotate (0-3).
#ifndef ROTATE
#define ROTATE 3
#endif

// Screen foreground/background colors.
#ifndef BG
#define BG BLACK
#endif
#ifndef FG
#define FG WHITE
#endif

// How often (milliseconds) to trigger the motor timer interrupt.
#ifndef MOTOR_INTERRUPT_MS
#define MOTOR_INTERRUPT_MS 10
#endif

// The maximum speed/acceleration of the motors.
#ifndef SPEED_STEPS_PER_SEC
#define SPEED_STEPS_PER_SEC 200.0
#endif
#ifndef ACCEL_STEPS_PER_SEC
#define ACCEL_STEPS_PER_SEC 100.0
#endif

// Whether to display seconds.
#ifndef DISPLAY_SEC
#define DISPLAY_SEC 0
#endif

#endif /* CONFIG_H */
