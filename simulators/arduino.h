#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

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
    assert (pins[pin].mode == INPUT);
    return LOW;
}

void digitalWrite(int pin, int val) {
    assert (pins[pin].mode == OUTPUT);
    pins[pin].val = val;
}

int bitRead(int val, int bit) {
    return (val >> bit) & 1;
}

// Initialization
void setup();
void loop();

static void arduino_init() {
    Serial.Serial = false;
    Serial.print = serial_print;
    Serial.begin = serial_begin;
    memset(Serial.out, '\0', SERIAL_OUT);
}

static void clear() {
    printf("\x1b[2J");
}

static void print_pin(struct pin_t *pin) {
    if (pin->mode == OUTPUT) {
        printf(" %s ", pin->val == HIGH ? "+" : "-");
    } else {
        printf("   ");
    }
}

static void display() {
    static unsigned long iter = 0;
    int i = 0;

    clear();
    if (Serial.out != NULL) {
        printf("%s\n\n", Serial.out);
    }
    printf("Iteration:  %lu\nTime (sec): %lu\n\n", iter, iter / 1000);

    for (i = 0; i < NPINS; i++) {
        printf("%2d ", i);
    }
    printf("\n");
    for (i = 0; i < NPINS; i++) {
        print_pin(&pins[i]);
    }
    printf("\n");

    iter += 1;
}

int main(int argc, char **argv) {
    double factor = 1 < argc ? atof(argv[1]) : 1.0;
    arduino_init();
    setup();
    while (1) {
        loop();
        display();
        usleep((unsigned int) (1000 / factor));
    }
    return 0;
}
