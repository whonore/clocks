#ifndef FONT_H
#define FONT_H

#include "clock.h"

// Hack to compute the correct file name for the current resolution.
// See:
// https://stackoverflow.com/questions/41766131/c-preprocessor-to-compute-product-specific-include
#define STR2(X)     #X
#define STR(X)      STR2(X)
#define FONT(digit) STR(fonts/digit-RESOLUTION.h)

#include FONT(0)
#include FONT(1)
#include FONT(2)
#include FONT(3)
#include FONT(4)
#include FONT(5)
#include FONT(6)
#include FONT(7)
#include FONT(8)
#include FONT(9)

const Point *const font[] =
  {pts_0, pts_1, pts_2, pts_3, pts_4, pts_5, pts_6, pts_7, pts_8, pts_9};
const size_t font_len[] = {pts_0_len,
                           pts_1_len,
                           pts_2_len,
                           pts_3_len,
                           pts_4_len,
                           pts_5_len,
                           pts_6_len,
                           pts_7_len,
                           pts_8_len,
                           pts_9_len};

#endif /* FONT_H */
