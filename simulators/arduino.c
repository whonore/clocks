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
void serial_print(const char *str) {
    int len = strnlen(Serial._out, SERIAL_OUT);
    if (len == SERIAL_OUT - 1) {
        memset(Serial._out, '\0', SERIAL_OUT);
        len = 0;
    }
    strncat(Serial._out, str, SERIAL_OUT - len - 1);
}

void serial_begin(unsigned int baud) {
    Serial.Serial = true;
}

// Library
static double time_factor;
static unsigned long usec_offset;

#define SCALE_TIME(usec) ((unsigned long) (_micros() * time_factor))

static unsigned long _micros() {
    struct timespec ticks;
    assert(clock_gettime(CLOCK_MONOTONIC, &ticks) == 0);
    return (ticks.tv_sec * 1000000) + (ticks.tv_nsec / 1000) - usec_offset;
}

unsigned long micros() {
    return SCALE_TIME(_micros());
}

unsigned long millis() {
    return micros() / 1000;
}

void delay(unsigned long ms) {
    struct timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = (ms - time.tv_sec * 1000) * 1000000;
    assert(nanosleep(&time, NULL) == 0);
}

void pinMode(uint8_t pin, uint8_t mode) {
    pins[pin].mode = mode;
}

int digitalRead(uint8_t pin) {
    int val;
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

    while (1) {
        pin = getch();
        if (pin == 'q') {
            quit = true;
            break;
        }
        if ('A' <= pin && pin <= 'A' + NPINS) {
            pin -= 'A';
            if (pins[pin].mode == INPUT) {
                pins[pin].val = HIGH;
            }
        }
    }

    return NULL;
}

// Initialization
static void serial_init() {
    Serial.Serial = false;
    Serial.print = serial_print;
    Serial.begin = serial_begin;
    memset(Serial._out, '\0', SERIAL_OUT);
}

static void pins_init() {
    int i;
    for (i = 0; i < NPINS; i++) {
        pins[i].mode = OUTPUT;
        pins[i].val = LOW;
    }
}

static void arduino_init() {
    serial_init();
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
    unsigned long usec = _micros();
    unsigned long usec_scaled = SCALE_TIME(usec);
    int i = 0;
    int y, x;
    getmaxyx(stdscr, y, x);
    int left = x / 2 - 3 * NPINS / 2;

    mvprintw(y / 2 - 5, left, "Time (simulated): %lu", usec_scaled / 1000000);
    mvprintw(y / 2 - 4, left, "Time (true):      %lu", usec / 1000000);

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
