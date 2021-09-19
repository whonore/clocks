#ifndef ADAFRUIT_SSD1351_H
#define ADAFRUIT_SSD1351_H

#include <arduino.h>
#include <ncurses.h>

class Adafruit_SSD1351 {
    private:
        uint16_t width;
        uint16_t height;
        uint8_t rotation = 0;

        void startWrite() {}
        void endWrite() { refresh(); }

        void writePixel(int16_t x, int16_t y, uint16_t color) {
            int width, height;
            getmaxyx(stdscr, height, width);

            if ((x < 0) || (y < 0) || (x >= this->width) || (y >= this->height)
                || (x >= width) || (y >= height)) {
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

            mvaddch(y, x, color != 0 ? 'X' : ' ');
        }
    public:
        Adafruit_SSD1351(uint16_t width, uint16_t height,
                         int8_t UNUSED(cs_pin), int8_t UNUSED(dc_pin),
                         int8_t UNUSED(mosi_pin), int8_t UNUSED(sclk_pin),
                         int8_t UNUSED(rst_pin)) {
            this->width = width;
            this->height = height;
        }

        void begin() {
            initscr();
            cbreak();
            noecho();
            curs_set(0);
        }

        void setRotation(uint8_t x) {
            this->rotation = x & 3;
            uint16_t w = this->width;
            uint16_t h = this->height;
            switch (rotation) {
                case 2:
                case 3:
                    this->width = h;
                    this->height = w;
                    break;
            }
        }

        void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
                        int16_t h, uint16_t color, uint16_t bg) {
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
};

#endif /* ADAFRUIT_SSD1351_H */
