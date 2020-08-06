#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "arduino.h"

struct pin_t { uint8_t mode; uint8_t val; };
static struct pin_t pins[NPINS];

// Serial
Serial_ Serial;

void Serial_::print(const char *str) {
    int len = strnlen(this->_out, SERIAL_OUT);
    if (len == SERIAL_OUT - 1) {
        memset(this->_out, '\0', SERIAL_OUT);
        len = 0;
    }
    strncat(this->_out, str, SERIAL_OUT - len - 1);
}

void Serial_::begin(unsigned int baud) {
    this->init = true;
}

Serial_::operator bool() {
    return this->init;
}

// Library
static double time_factor;
static uint32_t usec_offset;

#define SCALE_TIME(usec) ((uint32_t) (_micros() * time_factor))

static uint32_t _micros() {
    struct timespec ticks;
    assert(clock_gettime(CLOCK_MONOTONIC, &ticks) == 0);
    return (ticks.tv_sec * 1000000) + (ticks.tv_nsec / 1000) - usec_offset;
}

uint32_t micros() {
    return SCALE_TIME(_micros());
}

uint32_t millis() {
    return micros() / 1000;
}

void delay(uint32_t ms) {
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = (ms - time.tv_sec * 1000) * 1000000;
    assert(nanosleep(&time, NULL) == 0);
}

void pinMode(uint8_t pin, uint8_t mode) {
    pins[pin].mode = mode;
}

uint8_t digitalRead(uint8_t pin) {
    uint8_t val;
    assert(pins[pin].mode == INPUT);
    val = pins[pin].val;
    pins[pin].val = LOW;
    return val;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    assert(pins[pin].mode == OUTPUT);
    pins[pin].val = val;
}

// Reading input
static volatile bool quit = false;

static void *getkb(void *_arg) {
    int pin;

    while (!quit) {
        pin = getch();
        switch (pin) {
            case 'q':
                quit = true;
                break;
            case '+':
                time_factor += 1;
                break;
            case '-':
                time_factor -= 1;
                break;
            case ' ':
                time_factor = 1;
                break;
            default:
                if ('A' <= pin && pin <= 'A' + NPINS) {
                    pin -= 'A';
                    if (pins[pin].mode == INPUT) {
                        pins[pin].val = HIGH;
                    }
                }
        }
    }

    return NULL;
}

// Initialization
static void pins_init() {
    int i;
    for (i = 0; i < NPINS; i++) {
        pins[i].mode = OUTPUT;
        pins[i].val = LOW;
    }
}

static void arduino_init() {
    pins_init();
}

static void print_pin(int i) {
    int x, y;
    int c;
    getyx(stdscr, y, x);

    mvaddch(y - 1, x + 1, 'A' + i);

    if (i < A0) {
        mvprintw(y, x, "%2d", i);
    } else {
        mvprintw(y, x, "A%d", i - A0);
    }

    c = (pins[i].mode == OUTPUT)
        ? ('v' | ((pins[i].val == HIGH) ? A_REVERSE : 0))
        : '^';
    mvaddch(y + 1, x + 1, c);
    move(y, x + 3);
}

static void display() {
    uint32_t usec = _micros();
    uint32_t usec_scaled = SCALE_TIME(usec);
    int i = 0;
    int y, x;
    getmaxyx(stdscr, y, x);
    int left = x / 2 - 3 * NPINS / 2;

    mvprintw(y / 2 - 5, left, "Time (simulated x%.2f): %u",
             time_factor, usec_scaled / 1000000);
    mvprintw(y / 2 - 4, left, "Time (true):              %u",
             usec / 1000000);

    move(y / 2 - 1, left);
    for (i = 0; i < NPINS; i++) {
        print_pin(i);
    }

    if (Serial._out != NULL) {
        move(y / 2 + 5, 0);
        clrtobot();
        printw("%s", Serial._out);
    }

    refresh();
}

int main(int argc, char **argv) {
    time_factor = 1 < argc ? atof(argv[1]) : 1.0;
    usec_offset = _micros();
    pthread_t tid;

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    arduino_init();
    assert(pthread_create(&tid, NULL, &getkb, NULL) == 0);
    setup();
    while (1) {
        loop();
        display();
        if (quit) {
            assert(pthread_join(tid, NULL) == 0);
            break;
        }
        delay(100);
    }

    endwin();
    return 0;
}
