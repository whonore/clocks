#ifndef CONFIG_H
#define CONFIG_H

// The resolution of the digit font in pixels.
#ifndef RESOLUTION
#define RESOLUTION 16
#endif

// The screen dimensions in pixels.
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128;
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 128;
#endif

static_assert((SCREEN_WIDTH * SCREEN_HEIGHT) % 8 == 0,
              "SCREEN_WIDTH * SCREEN_HEIGHT must be a multiple of 8");

// How large to draw each pixel.
#ifndef SCALE
#define SCALE 3
#endif

// The space between digits in pixels.
#ifndef DIGIT_GAP
#define DIGIT_GAP 16
#endif

// Multiple of 90 degrees to rotate (0-3)
#ifndef ROTATE
#define ROTATE 3
#endif

#endif /* CONFIG_H */
