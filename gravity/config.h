#ifndef CONFIG_H
#define CONFIG_H

#include "local.h"

#ifndef DEBUG
#  define DEBUG 0
#endif

// The number of steps to offset from the zero switch.
#ifndef MIN_ZOFF
#  error "Must define MIN_ZOFF in local.h."
#endif
#ifndef HOUR_ZOFF
#  error "Must define HOUR_ZOFF in local.h."
#endif

// The resolution of the digit font in pixels.
#ifndef RESOLUTION
#  define RESOLUTION 16
#endif

// The screen dimensions in pixels.
#ifndef SCREEN_WIDTH
#  define SCREEN_WIDTH 128
#endif
#ifndef SCREEN_HEIGHT
#  define SCREEN_HEIGHT 128
#endif
static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) % 8 == 0,
              "SCREEN_WIDTH * SCREEN_HEIGHT must be a multiple of 8");

// How many pixels to shift the screen center (relative to ROTATE = 0).
#ifndef SCREEN_OFF_X
#  define SCREEN_OFF_X -4
#endif
#ifndef SCREEN_OFF_Y
#  define SCREEN_OFF_Y 11
#endif

// How large to draw each pixel.
#ifndef SCALE
#  define SCALE 3
#endif

// The space between digits in pixels.
#ifndef DIGIT_GAP
#  define DIGIT_GAP 16
#endif

// Multiple of 90 degrees to rotate (0-3).
#ifndef ROTATE
#  define ROTATE 3
#endif

// Screen foreground/background colors.
#ifndef BG
#  define BG BLACK
#endif
#ifndef FG
#  define FG WHITE
#endif

// How often (milliseconds) to trigger the motor timer interrupt.
#ifndef MOTOR_INTERRUPT_MS
#  define MOTOR_INTERRUPT_MS 10
#endif

// The maximum speed/acceleration of the motors.
#ifndef SPEED_STEPS_PER_SEC
#  define SPEED_STEPS_PER_SEC 200.0
#endif
#ifndef ACCEL_STEPS_PER_SEC
#  define ACCEL_STEPS_PER_SEC 100.0
#endif

// The number of degrees the hands turn per step of the motor.
#ifndef DEGREES_PER_STEP
#  define DEGREES_PER_STEP 1
#endif

// Whether to display seconds.
#ifndef DISPLAY_SEC
#  define DISPLAY_SEC 0
#endif

#if DISPLAY_SEC
// Second indicator color.
#  ifndef SEC_FG
#    define SEC_FG FG
#  endif

// How thick (in pixels) to draw the second indicator.
#  ifndef SEC_THICKNESS
#    define SEC_THICKNESS 3
#  endif
static_assert(1 <= SEC_THICKNESS, "SEC_THICKNESS must be at least 1");

// How many pixels from the edge to shift the second indicator
// (relative to ROTATE = 0).
#  ifndef SEC_MARGIN_TOP
#    define SEC_MARGIN_TOP 6
#  endif
#  ifndef SEC_MARGIN_RIGHT
#    define SEC_MARGIN_RIGHT 6
#  endif
#  ifndef SEC_MARGIN_BOTTOM
#    define SEC_MARGIN_BOTTOM 0
#  endif
#  ifndef SEC_MARGIN_LEFT
#    define SEC_MARGIN_LEFT 0
#  endif
#endif

#endif /* CONFIG_H */
