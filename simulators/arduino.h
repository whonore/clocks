#ifndef ARDUINO_H
#define ARDUINO_H 1

#include <assert.h>
#include <math.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
#define NPINS 20
struct pin_t { int mode; int val; };
static struct pin_t pins[NPINS];

// Serial
#define SERIAL_OUT 256
struct Serial_t {
    bool Serial;
    void (*print)(const char *str);
    void (*begin)(int baud);
    char out[SERIAL_OUT];
} Serial;

void serial_print(const char *str) {
    int len = strnlen(Serial.out, SERIAL_OUT);
    if (len == SERIAL_OUT - 1) {
        memset(Serial.out, '\0', SERIAL_OUT);
        len = 0;
    }
    strncat(Serial.out, str, SERIAL_OUT - len - 1);
}

void serial_begin(int baud) {
    Serial.Serial = true;
}

// Library
unsigned long millis() {
    static unsigned long time = 0;
    time += 1;
    return time;
}

void delay(int ms) {
    sleep((int) ceil(ms / 1000.0));
}

void pinMode(int pin, int mode) {
    pins[pin].mode = mode;
}

int digitalRead(int pin) {
    int val;
    assert(pins[pin].mode == INPUT);
    val = pins[pin].val;
    pins[pin].val = LOW;
    return val;
}

void digitalWrite(int pin, int val) {
    assert(pins[pin].mode == OUTPUT);
    pins[pin].val = val;
}

int bitRead(int val, int bit) {
    return (val >> bit) & 1;
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
void setup();
void loop();

static void serial_init() {
    Serial.Serial = false;
    Serial.print = serial_print;
    Serial.begin = serial_begin;
    memset(Serial.out, '\0', SERIAL_OUT);
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
    static unsigned long ticks = 0;
    int i = 0;
    int y, x;
    getmaxyx(stdscr, y, x);
    int left = x / 2 - 3 * NPINS / 2;

    mvprintw(y / 2 - 5, left, "Ticks:      %lu", ticks);
    mvprintw(y / 2 - 4, left, "Time (sec): %lu", ticks / 1000);

    move(y / 2 - 1, left);
    for (i = 0; i < NPINS; i++) {
        print_pin(i);
    }

    if (Serial.out != NULL) {
        move(y / 2 + 5, 0);
        clrtobot();
        printw("%s", Serial.out);
    }

    refresh();
    ticks += 1;
}

int main(int argc, char **argv) {
    double factor = 1 < argc ? atof(argv[1]) : 1.0;
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
        usleep((unsigned int) (1000 / factor));
        if (quit) {
            assert(pthread_join(tid, NULL) == 0);
            break;
        }
    }

    endwin();
    return 0;
}

#endif // ARDUINO_H
