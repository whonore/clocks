#ifndef ADAFRUIT_SSD1351_H
#define ADAFRUIT_SSD1351_H

#include <arduino.h>
#include <ncurses.h>

#ifndef _swap_int16_t
#  define _swap_int16_t(a, b) \
    {                         \
      int16_t t = a;          \
      a = b;                  \
      b = t;                  \
    }
#endif

static unsigned int nwindows = 0;

class Adafruit_SSD1351 {
  private:
    WINDOW *win = NULL;
    uint16_t width;
    uint16_t height;
    uint8_t rotation = 0;

    void startWrite() {}
    void endWrite() {
        box(this->win, 0, 0);
        wrefresh(this->win);
    }

    void writePixel(int16_t x, int16_t y, uint16_t color) {
        if ((x < 0) || (y < 0) || (x >= this->width) || (y >= this->height)) {
            return;
        }

        int16_t t;
        switch (this->rotation) {
        case 1:
            t = x;
            x = this->width - 1 - y;
            y = t;
            break;
        case 2:
            x = this->width - 1 - x;
            y = this->height - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = this->height - 1 - t;
            break;
        }

        // cppcheck-suppress shadowVariable
        int width, height;
        getmaxyx(this->win, height, width);
        if ((x >= width) || (y >= height)) {
            return;
        }

        mvwaddch(this->win, y, x, color != 0 ? 'X' : ' ');
    }

  public:
    Adafruit_SSD1351(uint16_t width,
                     uint16_t height,
                     int8_t UNUSED(cs_pin),
                     int8_t UNUSED(dc_pin),
                     int8_t UNUSED(mosi_pin),
                     int8_t UNUSED(sclk_pin),
                     int8_t UNUSED(rst_pin)) {
        this->width = width;
        this->height = height;
    }

    ~Adafruit_SSD1351() {
        nwindows--;
        delwin(this->win);
        if (nwindows == 0) {
            endwin();
        }
    }

    void begin() {
        if (nwindows == 0) {
            initscr();
            cbreak();
            noecho();
            curs_set(0);
        }
        this->win =
          newwin(this->height, this->width, 0, this->width * nwindows);
        nwindows++;
    }

    void setRotation(uint8_t x) {
        this->rotation = x & 3;
        uint16_t w = this->width;
        uint16_t h = this->height;
        switch (rotation) {
        case 1:
        case 3:
            this->width = h;
            this->height = w;
            break;
        }
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        startWrite();
        writePixel(x, y, color);
        endWrite();
    }

    void drawLine(int16_t x0,
                  int16_t y0,
                  int16_t x1,
                  int16_t y1,
                  uint16_t color) {
        int16_t steep = abs(y1 - y0) > abs(x1 - x0);
        if (steep) {
            _swap_int16_t(x0, y0);
            _swap_int16_t(x1, y1);
        }

        if (x0 > x1) {
            _swap_int16_t(x0, x1);
            _swap_int16_t(y0, y1);
        }

        int16_t dx, dy;
        dx = x1 - x0;
        dy = abs(y1 - y0);

        int16_t err = dx / 2;
        int16_t ystep;

        if (y0 < y1) {
            ystep = 1;
        } else {
            ystep = -1;
        }

        startWrite();
        for (; x0 <= x1; x0++) {
            if (steep) {
                writePixel(y0, x0, color);
            } else {
                writePixel(x0, y0, color);
            }
            err -= dy;
            if (err < 0) {
                y0 += ystep;
                err += dx;
            }
        }
        endWrite();
    }

    void drawBitmap(int16_t x,
                    int16_t y,
                    const uint8_t *bitmap,
                    int16_t w,
                    int16_t h,
                    uint16_t color,
                    uint16_t bg) {
        int16_t byteWidth = (w + 7) / 8;
        uint8_t byte = 0;

        startWrite();
        for (int16_t j = 0; j < h; j++, y++) {
            for (int16_t i = 0; i < w; i++) {
                if (i & 7) {
                    byte <<= 1;
                } else {
                    byte = bitmap[j * byteWidth + i / 8];
                }
                writePixel(x + i, y, (byte & 0x80) ? color : bg);
            }
        }
        endWrite();
    }

    void drawBitmap(int16_t x,
                    int16_t y,
                    const uint8_t *bitmap,
                    int16_t w,
                    int16_t h,
                    uint16_t color) {
        int16_t byteWidth = (w + 7) / 8;
        uint8_t byte = 0;

        startWrite();
        for (int16_t j = 0; j < h; j++, y++) {
            for (int16_t i = 0; i < w; i++) {
                if (i & 7) {
                    byte <<= 1;
                } else {
                    byte = bitmap[j * byteWidth + i / 8];
                }
                if (byte & 0x80) {
                    writePixel(x + i, y, color);
                }
            }
        }
        endWrite();
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        for (int16_t r = y; r < y + h; r++) {
            for (int16_t c = x; c < x + w; c++) {
                writePixel(c, r, color);
            }
        }
    }

    void fillScreen(uint16_t color) {
        this->fillRect(0, 0, this->width, this->height, color);
    }
};

#endif /* ADAFRUIT_SSD1351_H */
